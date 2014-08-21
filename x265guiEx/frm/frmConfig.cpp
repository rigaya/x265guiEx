//  -----------------------------------------------------------------------------------------
//    拡張 x264/x265 出力(GUI) Ex  v1.xx/2.xx/3.xx by rigaya
//  -----------------------------------------------------------------------------------------
//   ソースコードについて
//   ・無保証です。
//   ・本ソースコードを使用したことによるいかなる損害・トラブルについてrigayaは責任を負いません。
//   以上に了解して頂ける場合、本ソースコードの使用、複製、改変、再頒布を行って頂いて構いません。
//  -----------------------------------------------------------------------------------------

//以下warning C4100を黙らせる
//C4100 : 引数は関数の本体部で 1 度も参照されません。
#pragma warning( push )
#pragma warning( disable: 4100 )

#include "auo_version.h"
#include "auo_frm.h"
#include "auo_faw2aac.h"
#include "frmConfig.h"
#include "frmSaveNewStg.h"
#include "frmOtherSettings.h"
#include "frmBitrateCalculator.h"

using namespace x265guiEx;

/// -------------------------------------------------
///     設定画面の表示
/// -------------------------------------------------
[STAThreadAttribute]
void ShowfrmConfig(CONF_GUIEX *conf, const SYSTEM_DATA *sys_dat) {
	if (!sys_dat->exstg->s_local.disable_visual_styles)
		System::Windows::Forms::Application::EnableVisualStyles();
	System::IO::Directory::SetCurrentDirectory(String(sys_dat->aviutl_dir).ToString());
	frmConfig frmConf(conf, sys_dat);
	frmConf.ShowDialog();
}

/// -------------------------------------------------
///     frmSaveNewStg 関数
/// -------------------------------------------------
System::Boolean frmSaveNewStg::checkStgFileName(String^ stgName) {
	String^ fileName;
	if (stgName->Length == 0)
		return false;
	
	if (!ValidiateFileName(stgName)) {
		MessageBox::Show(L"ファイル名に使用できない文字が含まれています。\n保存できません。", L"エラー", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
	}
	if (String::Compare(Path::GetExtension(stgName), L".stg", true))
		stgName += L".stg";
	if (File::Exists(fileName = Path::Combine(fsnCXFolderBrowser->GetSelectedFolder(), stgName)))
		if (MessageBox::Show(stgName + L" はすでに存在します。上書きしますか?", L"上書き確認", MessageBoxButtons::YesNo, MessageBoxIcon::Question)
			!= System::Windows::Forms::DialogResult::Yes)
			return false;
	StgFileName = fileName;
	return true;
}

System::Void frmSaveNewStg::setStgDir(String^ _stgDir) {
	StgDir = _stgDir;
	fsnCXFolderBrowser->SetRootDirAndReload(StgDir);
}


/// -------------------------------------------------
///     frmBitrateCalculator 関数
/// -------------------------------------------------
System::Void frmBitrateCalculator::Init(int VideoBitrate, int AudioBitrate, bool BTVBEnable, bool BTABEnable, int ab_max) {
	guiEx_settings exStg(true);
	exStg.load_fbc();
	enable_events = false;
	fbcTXSize->Text = exStg.s_fbc.initial_size.ToString("F2");
	fbcChangeTimeSetMode(exStg.s_fbc.calc_time_from_frame != 0);
	fbcRBCalcRate->Checked = exStg.s_fbc.calc_bitrate != 0;
	fbcRBCalcSize->Checked = !fbcRBCalcRate->Checked;
	fbcTXMovieFrameRate->Text = Convert::ToString(exStg.s_fbc.last_fps);
	fbcNUMovieFrames->Value = exStg.s_fbc.last_frame_num;
	fbcNULengthHour->Value = Convert::ToDecimal((int)exStg.s_fbc.last_time_in_sec / 3600);
	fbcNULengthMin->Value = Convert::ToDecimal((int)(exStg.s_fbc.last_time_in_sec % 3600) / 60);
	fbcNULengthSec->Value =  Convert::ToDecimal((int)exStg.s_fbc.last_time_in_sec % 60);
	SetBTVBEnabled(BTVBEnable);
	SetBTABEnabled(BTABEnable, ab_max);
	SetNUVideoBitrate(VideoBitrate);
	SetNUAudioBitrate(AudioBitrate);
	enable_events = true;
}
System::Void frmBitrateCalculator::frmBitrateCalculator_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
	guiEx_settings exStg(true);
	exStg.load_fbc();
	exStg.s_fbc.calc_bitrate = fbcRBCalcRate->Checked;
	exStg.s_fbc.calc_time_from_frame = fbcPNMovieFrames->Visible;
	exStg.s_fbc.last_fps = Convert::ToDouble(fbcTXMovieFrameRate->Text);
	exStg.s_fbc.last_frame_num = Convert::ToInt32(fbcNUMovieFrames->Value);
	exStg.s_fbc.last_time_in_sec = Convert::ToInt32(fbcNULengthHour->Value) * 3600
		                         + Convert::ToInt32(fbcNULengthMin->Value) * 60
								 + Convert::ToInt32(fbcNULengthSec->Value);
	if (fbcRBCalcRate->Checked)
		exStg.s_fbc.initial_size = Convert::ToDouble(fbcTXSize->Text);
	exStg.save_fbc();
	frmConfig^ fcg = dynamic_cast<frmConfig^>(this->Owner);
	if (fcg != nullptr)
		fcg->InformfbcClosed();
}
System::Void frmBitrateCalculator::fbcRBCalcRate_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	if (fbcRBCalcRate->Checked && Convert::ToDouble(fbcTXSize->Text) <= 0.0) {
		guiEx_settings exStg(true);
		exStg.load_fbc();
		fbcTXSize->Text = exStg.s_fbc.initial_size.ToString("F2");
	}
}
System::Void frmBitrateCalculator::fbcBTVBApply_Click(System::Object^  sender, System::EventArgs^  e) {
	frmConfig^ fcg = dynamic_cast<frmConfig^>(this->Owner);
	if (fcg != nullptr)
		fcg->SetVideoBitrate((int)fbcNUBitrateVideo->Value);
}
System::Void frmBitrateCalculator::fbcBTABApply_Click(System::Object^  sender, System::EventArgs^  e) {
	frmConfig^ fcg = dynamic_cast<frmConfig^>(this->Owner);
	if (fcg != nullptr)
		fcg->SetAudioBitrate((int)fbcNUBitrateAudio->Value);
}


/// -------------------------------------------------
///     frmConfig 関数  (frmBitrateCalculator関連)
/// -------------------------------------------------
System::Void frmConfig::CloseBitrateCalc() {
	frmBitrateCalculator::Instance::get()->Close();
}
System::Void frmConfig::fcgTSBBitrateCalc_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	if (fcgTSBBitrateCalc->Checked) {
		int videoBitrate = 0;
		bool videoBitrateMode = fcgCXX265Mode->SelectedIndex == X265_RC_BITRATE;
		videoBitrateMode &= Int32::TryParse(fcgTXQuality->Text, videoBitrate);

		frmBitrateCalculator::Instance::get()->Init(
			(videoBitrateMode) ? videoBitrate : 0,
			(fcgNUAudioBitrate->Visible) ? (int)fcgNUAudioBitrate->Value : 0,
			videoBitrateMode,
			fcgNUAudioBitrate->Visible,
			(int)fcgNUAudioBitrate->Maximum
			);
		frmBitrateCalculator::Instance::get()->Owner = this;
		frmBitrateCalculator::Instance::get()->Show();
	} else {
		frmBitrateCalculator::Instance::get()->Close();
	}
}
System::Void frmConfig::SetfbcBTABEnable(bool enable, int max) {
	frmBitrateCalculator::Instance::get()->SetBTABEnabled(fcgNUAudioBitrate->Visible, max);
}
System::Void frmConfig::SetfbcBTVBEnable(bool enable) {
	frmBitrateCalculator::Instance::get()->SetBTVBEnabled(enable);
}

System::Void frmConfig::SetVideoBitrate(int bitrate) {
	if (x265_encmode_to_RCint[fcgCXX265Mode->SelectedIndex] == X265_RC_BITRATE)
		fcgTXQuality->Text = bitrate.ToString();
}

System::Void frmConfig::SetAudioBitrate(int bitrate) {
	SetNUValue(fcgNUAudioBitrate, bitrate);
}
System::Void frmConfig::InformfbcClosed() {
	fcgTSBBitrateCalc->Checked = false;
}

/// -------------------------------------------------
///     frmUpdate 関数
/// -------------------------------------------------
System::Void frmConfig::fcgTSBUpdate_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
#if ENABLE_AUOSETUP
	if (fcgTSBUpdate->Checked)
		frmExeUpdate->Show();
	else
		frmExeUpdate->Visible = false; //Close()してしまうとfrmExeUpdateがDisposeされてしまう
#endif
}

System::Void frmConfig::fcgTSBUpdate_CheckFinished(String^ mes) {
#if ENABLE_AUOSETUP
	//更新をチェックした時間を保存する
	guiEx_settings stg;
	stg.load_encode_stg();
	GetCHARfromString(stg.s_local.last_update_check, _countof(stg.s_local.last_update_check), DateTime::Now.ToString());
	stg.save_local();

	if (this->InvokeRequired) {
		this->Invoke(gcnew x265guiEx::DelegateProcessFin(this, &frmConfig::fcgTSBUpdate_CheckFinished), mes);
		return;
	}
	if (auoSetupControl::checkIfUpdateAvailable(mes)) {
		fcgTSBUpdate->Text = L"更新*";
		fcgTSBUpdate->ForeColor = Color::MediumSlateBlue;
	}
#endif
}

System::Void frmConfig::fcgTSBUpdate_UpdateFinished(String^ mes) {
#if ENABLE_AUOSETUP
	if (this->InvokeRequired) {
		this->Invoke(gcnew x265guiEx::DelegateProcessFin(this, &frmConfig::fcgTSBUpdate_UpdateFinished), mes);
		return;
	}

	fcgTSBUpdate->Text = L"更新";
	fcgTSBUpdate->ForeColor = SystemColors::ControlText;
	
	//更新にしたがって表示を更新する
	guiEx_settings stg;
	stg.load_encode_stg();
	LocalStg.x265Path        = String(stg.s_x265.fullpath).ToString();
	LocalStg.x265Pathhighbit = String(stg.s_x265.fullpath_highbit).ToString();
	LocalStg.MP4MuxerPath    = String(stg.s_mux[MUXER_MP4].fullpath).ToString();
	LocalStg.TC2MP4Path      = String(stg.s_mux[MUXER_TC2MP4].fullpath).ToString();
	LocalStg.MP4RawPath      = String(stg.s_mux[MUXER_MP4_RAW].fullpath).ToString();
	LocalStg.audEncPath[DEFAULT_AUDIO_ENCODER] = String(stg.s_aud[DEFAULT_AUDIO_ENCODER].fullpath).ToString();

	SetLocalStg();
#endif
}

System::Void frmConfig::InformfruClosed() {
	fcgTSBUpdate->Checked = false;
}

#if ENABLE_AUOSETUP
System::Void frmUpdate::frmUpdate_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
	e->Cancel = true;
	this->Visible = false; //Close()してしまうとfrmExeUpdateがDisposeされてしまう
	frmConfig^ fcg = dynamic_cast<frmConfig^>(this->Owner);
	if (fcg != nullptr)
		fcg->InformfruClosed();
}
#endif

/// -------------------------------------------------
///     frmConfig 関数
/// -------------------------------------------------
/////////////   LocalStg関連  //////////////////////
System::Void frmConfig::LoadLocalStg() {
	guiEx_settings *_ex_stg = sys_dat->exstg;
	_ex_stg->load_encode_stg();
	LocalStg.x265ExeName     = String(_ex_stg->s_x265.filename).ToString();
	LocalStg.x265Path        = String(_ex_stg->s_x265.fullpath).ToString();
	LocalStg.x265Pathhighbit = String(_ex_stg->s_x265.fullpath_highbit).ToString();
	LocalStg.CustomTmpDir    = String(_ex_stg->s_local.custom_tmp_dir).ToString();
	LocalStg.CustomAudTmpDir = String(_ex_stg->s_local.custom_audio_tmp_dir).ToString();
	LocalStg.CustomMP4TmpDir = String(_ex_stg->s_local.custom_mp4box_tmp_dir).ToString();
	LocalStg.LastAppDir      = String(_ex_stg->s_local.app_dir).ToString();
	LocalStg.LastBatDir      = String(_ex_stg->s_local.bat_dir).ToString();
	LocalStg.MP4MuxerExeName = String(_ex_stg->s_mux[MUXER_MP4].filename).ToString();
	LocalStg.MP4MuxerPath    = String(_ex_stg->s_mux[MUXER_MP4].fullpath).ToString();
	LocalStg.MKVMuxerExeName = String(_ex_stg->s_mux[MUXER_MKV].filename).ToString();
	LocalStg.MKVMuxerPath    = String(_ex_stg->s_mux[MUXER_MKV].fullpath).ToString();
	LocalStg.TC2MP4ExeName   = String(_ex_stg->s_mux[MUXER_TC2MP4].filename).ToString();
	LocalStg.TC2MP4Path      = String(_ex_stg->s_mux[MUXER_TC2MP4].fullpath).ToString();
	LocalStg.MPGMuxerExeName = String(_ex_stg->s_mux[MUXER_MPG].filename).ToString();
	LocalStg.MPGMuxerPath    = String(_ex_stg->s_mux[MUXER_MPG].fullpath).ToString();
	LocalStg.MP4RawExeName   = String(_ex_stg->s_mux[MUXER_MP4_RAW].filename).ToString();
	LocalStg.MP4RawPath      = String(_ex_stg->s_mux[MUXER_MP4_RAW].fullpath).ToString();

	LocalStg.audEncName->Clear();
	LocalStg.audEncExeName->Clear();
	LocalStg.audEncPath->Clear();
	for (int i = 0; i < _ex_stg->s_aud_count; i++) {
		LocalStg.audEncName->Add(String(_ex_stg->s_aud[i].dispname).ToString());
		LocalStg.audEncExeName->Add(String(_ex_stg->s_aud[i].filename).ToString());
		LocalStg.audEncPath->Add(String(_ex_stg->s_aud[i].fullpath).ToString());
	}
	if (_ex_stg->s_local.large_cmdbox)
		fcgTXCmd_DoubleClick(nullptr, nullptr); //初期状態は縮小なので、拡大
}

System::Boolean frmConfig::CheckLocalStg() {
	bool error = false;
	String^ err = "";
	//x265のチェック
	bool CheckX265highbit;
	if (fcgTSBCMDOnly->Checked) {
		//CLIモードの時はコマンドラインを解析してhighbitかどうか判定
		CONF_GUIEX cnf;
		init_CONF_GUIEX(&cnf, FALSE);
		char cmdex[2048] = { 0 };
		GetCHARfromString(cmdex, sizeof(cmdex), fcgTXCmdEx->Text);
		set_cmd_to_conf(cmdex, &cnf.x265);
		CheckX265highbit = 8 < cnf.x265.bit_depth;
	} else {
		CheckX265highbit = fcgCBUsehighbit->Checked;
	}
	if (!CheckX265highbit && !File::Exists(LocalStg.x265Path)) {
		error = true;
		err += L"指定された x265 は存在しません。\n [ " + LocalStg.x265Path + L" ]\n";
	}
	if (CheckX265highbit && !File::Exists(LocalStg.x265Pathhighbit)) {
		error = true;
		err += L"指定された x265 (highbit用) は存在しません。\n [ " + LocalStg.x265Pathhighbit + L" ]\n";
	}
	//音声エンコーダのチェック (実行ファイル名がない場合はチェックしない)
	if (LocalStg.audEncExeName[fcgCXAudioEncoder->SelectedIndex]->Length) {
		String^ AudioEncoderPath = LocalStg.audEncPath[fcgCXAudioEncoder->SelectedIndex];
		if (!File::Exists(AudioEncoderPath) 
			&& (fcgCXAudioEncoder->SelectedIndex != sys_dat->exstg->s_aud_faw_index 
			    || !check_if_faw2aac_exists()) ) {
			//音声実行ファイルがない かつ
			//選択された音声がfawでない または fawであってもfaw2aacがない
			if (error) err += L"\n\n";
			error = true;
			err += L"指定された 音声エンコーダ は存在しません。\n [ " + AudioEncoderPath + L" ]\n";
		}
	}
	//FAWのチェック
	if (fcgCBFAWCheck->Checked) {
		if (sys_dat->exstg->s_aud_faw_index == FAW_INDEX_ERROR) {
			if (error) err += L"\n\n";
			error = true;
			err += L"FAWCheckが選択されましたが、x265guiEx.ini から\n"
				+ L"FAW の設定を読み込めませんでした。\n"
				+ L"x265guiEx.ini を確認してください。\n";
		} else if (!File::Exists(LocalStg.audEncPath[sys_dat->exstg->s_aud_faw_index])
			       && !check_if_faw2aac_exists()) {
			//fawの実行ファイルが存在しない かつ faw2aacも存在しない
			if (error) err += L"\n\n";
			error = true;
			err += L"FAWCheckが選択されましたが、FAW(fawcl)へのパスが正しく指定されていません。\n"
				+  L"一度設定画面でFAW(fawcl)へのパスを指定してください。\n";
		}
	}
	//自動マルチパスの自動ビットレート設定のチェック
	if (fcgLBAMPAutoBitrate != nullptr && fcgLBAMPAutoBitrate->Visible) {
		if (error) err += L"\n\n";
		error = true;
		err += L"目標映像ビットレートを自動に設定するには、\n"
			+ L"上限ビットレート、上限ファイルサイズの少なくとも片方を\n"
			+ L"適切に設定する必要があります。\n"
			+ L"上限ビットレート、上限ファイルサイズの設定を見なおしてください。";
	}
	if (error) 
		MessageBox::Show(this, err, L"エラー", MessageBoxButtons::OK, MessageBoxIcon::Error);
	return error;
}

System::Void frmConfig::SaveLocalStg() {
	guiEx_settings *_ex_stg = sys_dat->exstg;
	_ex_stg->load_encode_stg();
	_ex_stg->s_local.large_cmdbox = fcgTXCmd->Multiline;
	GetCHARfromString(_ex_stg->s_x265.fullpath,               sizeof(_ex_stg->s_x265.fullpath),               LocalStg.x265Path);
	GetCHARfromString(_ex_stg->s_x265.fullpath_highbit,       sizeof(_ex_stg->s_x265.fullpath_highbit),       LocalStg.x265Pathhighbit);
	GetCHARfromString(_ex_stg->s_local.custom_tmp_dir,        sizeof(_ex_stg->s_local.custom_tmp_dir),        LocalStg.CustomTmpDir);
	GetCHARfromString(_ex_stg->s_local.custom_mp4box_tmp_dir, sizeof(_ex_stg->s_local.custom_mp4box_tmp_dir), LocalStg.CustomMP4TmpDir);
	GetCHARfromString(_ex_stg->s_local.custom_audio_tmp_dir,  sizeof(_ex_stg->s_local.custom_audio_tmp_dir),  LocalStg.CustomAudTmpDir);
	GetCHARfromString(_ex_stg->s_local.app_dir,               sizeof(_ex_stg->s_local.app_dir),               LocalStg.LastAppDir);
	GetCHARfromString(_ex_stg->s_local.bat_dir,               sizeof(_ex_stg->s_local.bat_dir),               LocalStg.LastBatDir);
	GetCHARfromString(_ex_stg->s_mux[MUXER_MP4].fullpath,     sizeof(_ex_stg->s_mux[MUXER_MP4].fullpath),     LocalStg.MP4MuxerPath);
	GetCHARfromString(_ex_stg->s_mux[MUXER_MKV].fullpath,     sizeof(_ex_stg->s_mux[MUXER_MKV].fullpath),     LocalStg.MKVMuxerPath);
	GetCHARfromString(_ex_stg->s_mux[MUXER_TC2MP4].fullpath,  sizeof(_ex_stg->s_mux[MUXER_TC2MP4].fullpath),  LocalStg.TC2MP4Path);
	GetCHARfromString(_ex_stg->s_mux[MUXER_MPG].fullpath,     sizeof(_ex_stg->s_mux[MUXER_MPG].fullpath),     LocalStg.MPGMuxerPath);
	GetCHARfromString(_ex_stg->s_mux[MUXER_MP4_RAW].fullpath, sizeof(_ex_stg->s_mux[MUXER_MP4_RAW].fullpath), LocalStg.MP4RawPath);
	for (int i = 0; i < _ex_stg->s_aud_count; i++)
		GetCHARfromString(_ex_stg->s_aud[i].fullpath,         sizeof(_ex_stg->s_aud[i].fullpath),             LocalStg.audEncPath[i]);
	_ex_stg->save_local();
}

System::Void frmConfig::SetLocalStg() {
	fcgLBX265Path->Text           = L"x265.exe の指定";
	fcgLBX265PathSub->Text        = L"x265.exe の指定";
	fcgTXX265Path->Text           = (fcgCBUsehighbit->Checked) ? LocalStg.x265Pathhighbit : LocalStg.x265Path;
	fcgTXX265PathSub->Text        = LocalStg.x265Path;
	fcgTXX265PathSubhighbit->Text = LocalStg.x265Pathhighbit;

	fcgTXMP4MuxerPath->Text       = LocalStg.MP4MuxerPath;
	fcgTXMKVMuxerPath->Text       = LocalStg.MKVMuxerPath;
	fcgTXTC2MP4Path->Text         = LocalStg.TC2MP4Path;
	fcgTXMPGMuxerPath->Text       = LocalStg.MPGMuxerPath;
	fcgTXMP4RawPath->Text         = LocalStg.MP4RawPath;
	fcgTXCustomAudioTempDir->Text = LocalStg.CustomAudTmpDir;
	fcgTXCustomTempDir->Text      = LocalStg.CustomTmpDir;
	fcgTXMP4BoxTempDir->Text      = LocalStg.CustomMP4TmpDir;
	fcgLBMP4MuxerPath->Text       = LocalStg.MP4MuxerExeName + L" の指定";
	fcgLBMKVMuxerPath->Text       = LocalStg.MKVMuxerExeName + L" の指定";
	fcgLBTC2MP4Path->Text         = LocalStg.TC2MP4ExeName   + L" の指定";
	fcgLBMPGMuxerPath->Text       = LocalStg.MPGMuxerExeName + L" の指定";
	fcgLBMP4RawPath->Text         = LocalStg.MP4RawExeName + L" の指定";

	fcgTXX265Path->SelectionStart           = fcgTXX265Path->Text->Length;
	fcgTXX265PathSub->SelectionStart        = fcgTXX265PathSub->Text->Length;
	fcgTXX265PathSubhighbit->SelectionStart = fcgTXX265PathSubhighbit->Text->Length;
	fcgTXMP4MuxerPath->SelectionStart       = fcgTXMP4MuxerPath->Text->Length;
	fcgTXTC2MP4Path->SelectionStart         = fcgTXTC2MP4Path->Text->Length;
	fcgTXMKVMuxerPath->SelectionStart       = fcgTXMKVMuxerPath->Text->Length;
	fcgTXMPGMuxerPath->SelectionStart       = fcgTXMPGMuxerPath->Text->Length;
	fcgTXMP4RawPath->SelectionStart         = fcgTXMP4RawPath->Text->Length;
}

//////////////   TrackBar用タイマー関連     /////////////////////////
System::Void frmConfig::qualityTimerChangeX265(Object^ state) {
	this->Invoke(gcnew qualityTimerChangeDelegate(this, &frmConfig::fcgTBQualityChange));
}

System::Void frmConfig::fcgTBQualityChange() {
	int j = fcgTBQuality->Value;
	int i = j + timerChangeValue;
	i = clamp(i, fcgTBQuality->Minimum, fcgTBQuality->Maximum);
	fcgTBQuality->Value = i;
	if (i != j)
		fcgTBQuality_Scroll(nullptr, nullptr);
}

System::Void frmConfig::InitTimer() {
	qualityTimerX265 = gcnew System::Threading::Timer(
		gcnew System::Threading::TimerCallback(this, &frmConfig::qualityTimerChangeX265),
		nullptr, System::Threading::Timeout::Infinite, fcgTBQualityTimerPeriod);
	timerChangeValue = 1;
}


//////////////       その他イベント処理   ////////////////////////
System::Void frmConfig::ActivateToolTip(bool Enable) {
	fcgTTEx->Active = Enable;
	fcgTTX265->Active = Enable;
	fcgTTX265Version->Active = Enable;
}

System::Void frmConfig::fcgTSBOtherSettings_Click(System::Object^  sender, System::EventArgs^  e) {
	frmOtherSettings::Instance::get()->stgDir = String(sys_dat->exstg->s_local.stg_dir).ToString();
	frmOtherSettings::Instance::get()->ShowDialog();
	char buf[MAX_PATH_LEN];
	GetCHARfromString(buf, sizeof(buf), frmOtherSettings::Instance::get()->stgDir);
	if (_stricmp(buf, sys_dat->exstg->s_local.stg_dir)) {
		//変更があったら保存する
		strcpy_s(sys_dat->exstg->s_local.stg_dir, sizeof(sys_dat->exstg->s_local.stg_dir), buf);
		sys_dat->exstg->save_local();
		InitStgFileList();
	}
	//再読み込み
	guiEx_settings stg;
	stg.load_encode_stg();
	log_reload_settings();
	sys_dat->exstg->s_local.default_audio_encoder = stg.s_local.default_audio_encoder;
	sys_dat->exstg->s_local.get_relative_path = stg.s_local.get_relative_path;
	SetStgEscKey(stg.s_local.enable_stg_esc_key != 0);
	ActivateToolTip(stg.s_local.disable_tooltip_help == FALSE);
	if (str_has_char(stg.s_local.conf_font.name))
		SetFontFamilyToForm(this, gcnew FontFamily(String(stg.s_local.conf_font.name).ToString()), this->Font->FontFamily);
}


System::Void frmConfig::fcgTSBRearrageTabs_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	//CLIモードとの切り替え
	//一度ウィンドウの再描画を完全に抑止する
	SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 0, 0);
	int tabPageCount = fcgtabControlVideo->TabPages->Count;
	for (int i = 0; i < tabPageCount - 1; i++) {
		fcgtabControlVideo->TabPages->RemoveAt(0);
	}
	if (!fcgTSBCMDOnly->Checked) {
		fcgtabControlVideo->TabPages->Insert(0, fcgtabPageX265Main);
		fcgtabControlVideo->TabPages->Insert(1, fcgtabPageX265Other);
	}
	fcgtabControlVideo->SelectedIndex = 0;
	//一度ウィンドウの再描画を再開し、強制的に再描画させる
	SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 1, 0);
	this->Refresh();

	if (fcgTSBCMDOnly->Checked) {
		fcgtabPageExSettings->Text = L"映像";
		fcggroupBoxCmdEx->Text = L"コマンドライン";
	} else {
		fcgtabPageExSettings->Text = L"拡張";
		fcggroupBoxCmdEx->Text = L"追加コマンド";
	}

	fcgChangeEnabled(sender, e);
	fcgRebuildCmd(sender, e);
}

System::Void frmConfig::fcgCBUsehighbit_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	fcgCXX265Mode_SelectedIndexChanged(sender, e);
	fcgTXX265Path->Text = (fcgCBUsehighbit->Checked) ? LocalStg.x265Pathhighbit : LocalStg.x265Path;
	fcgTXX265Path->SelectionStart = fcgTXX265Path->Text->Length;
	fcgLBX265Path->Text = (fcgCBUsehighbit->Checked) ? L"x265.exe(highbit) の指定" : L"x265.exe の指定";
	SetX265VersionToolTip(fcgTXX265Path->Text, fcgCBUsehighbit->Checked);
	SetTBValueToTextBoxX265();
}

System::Void frmConfig::fcgCBAFS_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	fcgCBAFSBitrateCorrection->Enabled = fcgCBAFS->Checked;
	if (fcgCBAFS->Checked) {
		//fcgCBMP4MuxerExt->Checked      = true;
		//fcgCBMKVMuxerExt->Checked      = true;
	}
	//fcgCBMP4MuxerExt->Enabled          = !fcgCBAFS->Checked;
	//fcgCBMKVMuxerExt->Enabled          = !fcgCBAFS->Checked;

	bool disable_keyframe_detect = fcgCBAFS->Checked && !sys_dat->exstg->s_local.set_keyframe_as_afs_24fps;
	if (disable_keyframe_detect)
		fcgCBCheckKeyframes->Checked   = false;
	fcgCBCheckKeyframes->Enabled       = !disable_keyframe_detect;

	int muxer_cmdex = fcgCXMP4CmdEx->SelectedIndex;
	setMuxerCmdExNames(fcgCXMP4CmdEx, (fcgCBAFS->Checked) ? MUXER_TC2MP4 : MUXER_MP4);
	fcgCXMP4CmdEx->SelectedIndex = muxer_cmdex;
}
System::Void frmConfig::fcgArrangeForAutoMultiPass(bool enable) {
	const int PNX265ModeOffset = +7;
	const int PNBitrateOffset  = -39;
	const int PNStatusFileOffset = -17;
	if (fcgLastX265ModeAsAMP == enable)
		return;
	Point NewPoint = fcgPNX265Mode->Location;
	NewPoint.Y += PNX265ModeOffset * ((enable) ? -1 : 1);
	fcgPNX265Mode->Location = NewPoint;
	fcgPNX265Mode->Height += (PNBitrateOffset - PNX265ModeOffset*2) * ((enable) ? -1 : 1);
	NewPoint = fcgPNBitrate->Location;
	NewPoint.Y += PNBitrateOffset * ((enable) ? -1 : 1);
	fcgPNBitrate->Location = NewPoint;
	NewPoint = fcgPNStatusFile->Location;
	NewPoint.Y += PNStatusFileOffset * ((enable) ? -1 : 1);
	fcgPNStatusFile->Location = NewPoint;
	fcgLastX265ModeAsAMP = enable;
	fcgCBAMPLimitBitrate->Visible = enable;
	fcgCBAMPLimitFileSize->Visible = enable;
	fcgNUAMPLimitBitrate->Visible = enable;
	fcgNUAMPLimitFileSize->Visible = enable;
}

System::Void frmConfig::fcgCheckAMPAutoBitrateEvent(System::Object^  sender, System::EventArgs^  e) {
	if (fcgLBAMPAutoBitrate == nullptr)
		return;
	if (fcgCXX265Mode->SelectedIndex == 5) {
		if (   0 == String::Compare(fcgTXQuality->Text, STR_BITRATE_AUTO)
			|| 0 == String::Compare(fcgTXQuality->Text, L"-1")) {
				if (!fcgCBAMPLimitBitrate->Checked && !fcgCBAMPLimitFileSize->Checked) {
				fcgLBAMPAutoBitrate->Visible = true;
				return;
				}
		}
	}
	fcgLBAMPAutoBitrate->Visible = false;
}

System::Void frmConfig::AddCheckAMPAutoBitrateEvent() {
	fcgCBAMPLimitBitrate->CheckedChanged += gcnew System::EventHandler(this, &frmConfig::fcgCheckAMPAutoBitrateEvent);
	fcgCBAMPLimitFileSize->CheckedChanged += gcnew System::EventHandler(this, &frmConfig::fcgCheckAMPAutoBitrateEvent);
	fcgCXX265Mode->SelectedIndexChanged += gcnew System::EventHandler(this, &frmConfig::fcgCheckAMPAutoBitrateEvent);
	fcgTXQuality->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgCheckAMPAutoBitrateEvent);
}

System::Void frmConfig::fcgCXX265Mode_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	int index = fcgCXX265Mode->SelectedIndex;
	cnf_fcgTemp->rc_mode = x265_encmode_to_RCint[index];
	cnf_fcgTemp->use_auto_npass = (fcgCXX265Mode->SelectedIndex == 5 || fcgCXX265Mode->SelectedIndex == 6);
	switch (cnf_fcgTemp->rc_mode) {
		case X265_RC_BITRATE:
			fcgLBQuality->Text = (fcgCXX265Mode->SelectedIndex == 5) ? L"目標映像ビットレート(kbps, \"-1\"で自動)" : L"ビットレート(kbps)";
			fcgLBQualityLeft->Text = L"低品質";
			fcgLBQualityRight->Text = L"高品質";
			fcgTBQuality->Minimum = 0;
			fcgTBQuality->Maximum = TBBConvert.getMaxCount();
			cnf_fcgTemp->pass = x265_encmode_to_passint[index];
			if (fcgCXX265Mode->SelectedIndex >= 3) {
				fcgCBNulOut->Enabled = true;
				fcgCBNulOut->Checked = cnf_fcgTemp->nul_out != 0;
				if (fcgCXX265Mode->SelectedIndex == 4) {
					fcgCBFastFirstPass->Enabled = false; //Enabledの変更が先
					fcgCBFastFirstPass->Checked = false;
				} else {
					fcgCBFastFirstPass->Enabled = true; //Enabledの変更が先
					fcgCBFastFirstPass->Checked = !cnf_fcgTemp->slow_first_pass;
				}
			} else {
				fcgCBNulOut->Enabled = false; //Enabledの変更が先
				fcgCBNulOut->Checked = false;
				fcgCBFastFirstPass->Enabled = false; //Enabledの変更が先
				fcgCBFastFirstPass->Checked = false;
			}
			//自動(-1)から変更されたときの処置 → 1000に戻す
			if ((cnf_fcgTemp->bitrate == -1) && (fcgCXX265Mode->SelectedIndex != 5))
				cnf_fcgTemp->bitrate = 1000;
			//文字列を更新
			if ((cnf_fcgTemp->bitrate == -1) && (fcgCXX265Mode->SelectedIndex == 5))
				fcgTXQuality->Text = STR_BITRATE_AUTO; //-1の特例処置(-1: 自動)
			else
				fcgTXQuality->Text = Convert::ToString(cnf_fcgTemp->bitrate);
			SetfbcBTVBEnable(true);
			break;
		case X265_RC_QP:
			fcgLBQuality->Text = L"量子化量(Quantizer)";
			fcgLBQualityLeft->Text = L"高品質";
			fcgLBQualityRight->Text = L"低品質";
			fcgTBQuality->Minimum = -1;
			fcgTBQuality->Maximum = 69;
			fcgCBNulOut->Enabled = false; //Enabledの変更が先
			fcgCBNulOut->Checked = false;
			fcgCBFastFirstPass->Enabled = false; //Enabledの変更が先
			fcgCBFastFirstPass->Checked = false;
			fcgTXQuality->Text = Convert::ToString(cnf_fcgTemp->qp);
			SetfbcBTVBEnable(false);
			break;
		case X265_RC_CRF:
		default:
			fcgLBQuality->Text = L"品質(Quality)";
			fcgLBQualityLeft->Text = L"高品質";
			fcgLBQualityRight->Text = L"低品質";
			fcgTBQuality->Minimum = (fcgCBUsehighbit->Checked) ? -12*2 : 0;
			fcgTBQuality->Maximum = 51*2;
			fcgCBNulOut->Enabled = false; //Enabledの変更が先
			fcgCBNulOut->Checked = false;
			fcgCBFastFirstPass->Enabled = false; //Enabledの変更が先
			fcgCBFastFirstPass->Checked = false;
			fcgTXQuality->Text = Convert::ToString(cnf_fcgTemp->crf / 100.0);
			SetfbcBTVBEnable(false);
			break;
	}
	fcgNUAutoNPass->Enabled = (fcgCXX265Mode->SelectedIndex == 5);
	fcgArrangeForAutoMultiPass(cnf_fcgTemp->use_auto_npass != 0);
}

System::Void frmConfig::fcgTXQuality_Enter(System::Object^  sender, System::EventArgs^  e) {
	if (0 == String::Compare(fcgTXQuality->Text, STR_BITRATE_AUTO)) {
		fcgTXQuality->Text = L"-1";
		fcgTXQuality->Select(0, fcgTXQuality->Text->Length);
	}
}

System::Void frmConfig::fcgTXQuality_TextChanged(System::Object^  sender, System::EventArgs^  e) {
	if (fcgTXQuality->Text->Length == 0 || String::Compare(fcgTXQuality->Text, L"-") == 0)
		return;
	//自動モードの文字列に変更されたときの処理
	if (0 == String::Compare(fcgTXQuality->Text, STR_BITRATE_AUTO)) {
		fcgTXQuality->Text = STR_BITRATE_AUTO;
		cnf_fcgTemp->bitrate = -1;
		fcgTBQuality->Value = TBBConvert.BitrateToTB(cnf_fcgTemp->bitrate);
		lastQualityStr = fcgTXQuality->Text;
		fcgTXQuality->SelectionStart = fcgTXQuality->Text->Length;
		fcgTXQuality->SelectionLength = 0;
		return;
	}
	int c = fcgTXQuality->SelectionStart;
	int index = fcgCXX265Mode->SelectedIndex;
	bool restore = false;
	int i;
	double d;
	if (!Double::TryParse(fcgTXQuality->Text, d)) {
		fcgTXQuality->Text = lastQualityStr;
		restore = true;
	} else {
		switch (x265_encmode_to_RCint[index]) {
		case X265_RC_BITRATE:
			//自動マルチパス時は-1(自動)もあり得る
			if (Int32::TryParse(fcgTXQuality->Text, i) && i >= ((fcgCXX265Mode->SelectedIndex == 5) ? -1 : 0)) {
				cnf_fcgTemp->bitrate = i;
				fcgTXQuality->Text = i.ToString();
				fcgTBQuality->Value = TBBConvert.BitrateToTB(cnf_fcgTemp->bitrate);
			} else {
				fcgTXQuality->Text = lastQualityStr;
				restore = true;
			}
			break;
		case X265_RC_QP:
			if (Int32::TryParse(fcgTXQuality->Text, i)) {
				i = SetTBValue(fcgTBQuality, i);
				cnf_fcgTemp->qp = i;
				fcgTXQuality->Text = Convert::ToString(i);
			}
			break;
		case X265_RC_CRF:
		default:
			if (Double::TryParse(fcgTXQuality->Text, d)) {
				int TBmin = fcgTBQuality->Minimum * 50;
				int TBmax = fcgTBQuality->Maximum * 50;
				i = clamp((int)Math::Round(d * 100), TBmin, TBmax);
				cnf_fcgTemp->crf = i;
				//小数点以下3桁目カットを考慮
				if (d*1000 != cnf_fcgTemp->crf * 10)
					fcgTXQuality->Text = Convert::ToString(i / 100.0);
			}
			fcgTBQuality->Value = (int)Math::Round(cnf_fcgTemp->crf / 50);
			break;
		}
	}
	//カーソルの位置を動かさないように   復元したのなら、直前の入力は無効のハズ
	fcgTXQuality->SelectionStart = clamp(c - Convert::ToInt32(restore), 0, fcgTXQuality->Text->Length);
	fcgTXQuality->SelectionLength = 0;
	lastQualityStr = fcgTXQuality->Text;
}

System::Void frmConfig::fcgTXQuality_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
	switch (x265_encmode_to_RCint[fcgCXX265Mode->SelectedIndex]) {
		case X265_RC_BITRATE:
			//自動モードの場合は除く
			if (fcgCXX265Mode->SelectedIndex == 5 && cnf_fcgTemp->bitrate == -1) {
				fcgTXQuality->Text = STR_BITRATE_AUTO;
			} else
				fcgTXQuality->Text = Convert::ToString(cnf_fcgTemp->bitrate);
			break;
		case X265_RC_QP:
			fcgTXQuality->Text = Convert::ToString(cnf_fcgTemp->qp);
			break;
		case X265_RC_CRF:
			fcgTXQuality->Text = Convert::ToString(cnf_fcgTemp->crf / 100.0);
		default:
			break;
	}
}

System::Void frmConfig::SetTBValueToTextBoxX265() {
	int index = fcgCXX265Mode->SelectedIndex;
	switch (x265_encmode_to_RCint[index]) {
		case X265_RC_BITRATE:
			cnf_fcgTemp->bitrate = TBBConvert.TBToBitrate(fcgTBQuality->Value);
			fcgTXQuality->Text = Convert::ToString(cnf_fcgTemp->bitrate);
			break;
		case X265_RC_QP:
			cnf_fcgTemp->qp = fcgTBQuality->Value;
			fcgTXQuality->Text = Convert::ToString(cnf_fcgTemp->qp);
			break;
		case X265_RC_CRF:
		default:
			cnf_fcgTemp->crf = fcgTBQuality->Value * 50;
			fcgTXQuality->Text = Convert::ToString(cnf_fcgTemp->crf / 100.0);
			break;
	}
}
System::Boolean frmConfig::EnableSettingsNoteChange(bool Enable) {
	if (fcgTSTSettingsNotes->Visible == Enable &&
		fcgTSLSettingsNotes->Visible == !Enable)
		return true;
	if (CountStringBytes(fcgTSTSettingsNotes->Text) > fcgTSTSettingsNotes->MaxLength - 1) {
		MessageBox::Show(this, L"入力された文字数が多すぎます。減らしてください。", L"エラー", MessageBoxButtons::OK, MessageBoxIcon::Error);
		fcgTSTSettingsNotes->Focus();
		fcgTSTSettingsNotes->SelectionStart = fcgTSTSettingsNotes->Text->Length;
		return false;
	}
	fcgTSTSettingsNotes->Visible = Enable;
	fcgTSLSettingsNotes->Visible = !Enable;
	if (Enable) {
		fcgTSTSettingsNotes->Text = fcgTSLSettingsNotes->Text;
		fcgTSTSettingsNotes->Focus();
		bool isDefaultNote = String::Compare(fcgTSTSettingsNotes->Text, String(DefaultStgNotes).ToString()) == 0;
		fcgTSTSettingsNotes->Select((isDefaultNote) ? 0 : fcgTSTSettingsNotes->Text->Length, fcgTSTSettingsNotes->Text->Length);
	} else {
		SetfcgTSLSettingsNotes(fcgTSTSettingsNotes->Text);
		CheckOtherChanges(nullptr, nullptr);
	}
	return true;
}


///////////////////  メモ関連  ///////////////////////////////////////////////
System::Void frmConfig::fcgTSLSettingsNotes_DoubleClick(System::Object^  sender, System::EventArgs^  e) {
	EnableSettingsNoteChange(true);
}
System::Void frmConfig::fcgTSTSettingsNotes_Leave(System::Object^  sender, System::EventArgs^  e) {
	EnableSettingsNoteChange(false);
}
System::Void frmConfig::fcgTSTSettingsNotes_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
	if (e->KeyCode == Keys::Return)
		EnableSettingsNoteChange(false);
}
System::Void frmConfig::fcgTSTSettingsNotes_TextChanged(System::Object^  sender, System::EventArgs^  e) {
	SetfcgTSLSettingsNotes(fcgTSTSettingsNotes->Text);
	CheckOtherChanges(nullptr, nullptr);
}

//////////////////// 追加コマンド関連 /////////////////////////////////////////
System::Void frmConfig::fcgCXCmdExInsert_FontChanged(System::Object^  sender, System::EventArgs^  e) {
	InitCXCmdExInsert();
}
System::Void frmConfig::fcgCXCmdExInsert_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	String^ insertStr;
	if (       0 == fcgCXCmdExInsert->SelectedIndex) {
		//何もしない
	} else if (1 == fcgCXCmdExInsert->SelectedIndex) {			
		//WinXPにおいて、OpenFileDialogはCurrentDirctoryを勝手に変更しやがるので、
		//一度保存し、あとから再適用する
		String^ CurrentDir = Directory::GetCurrentDirectory();
		OpenFileDialog^ ofd = gcnew OpenFileDialog();
		ofd->FileName = L"";
		ofd->Multiselect = false;
		ofd->Filter = L"(*.*)|*.*";
		bool ret = (ofd->ShowDialog() == Windows::Forms::DialogResult::OK);
		if (ret) {
			if (sys_dat->exstg->s_local.get_relative_path)
				ofd->FileName = GetRelativePath(ofd->FileName, CurrentDir);
			insertStr = ofd->FileName;
		}
		Directory::SetCurrentDirectory(CurrentDir);
	} else {
		insertStr = String(REPLACE_STRINGS_LIST[fcgCXCmdExInsert->SelectedIndex-2].string).ToString();
	}
	if (insertStr != nullptr && insertStr->Length > 0) {
		int current_selection = fcgTXCmdEx->SelectionStart;
		fcgTXCmdEx->Text = fcgTXCmdEx->Text->Insert(fcgTXCmdEx->SelectionStart, insertStr);
		fcgTXCmdEx->SelectionStart = current_selection + insertStr->Length; //たまに変なところへ行くので念のため必要
		fcgTXCmdEx->Focus();
	}
	fcgCXCmdExInsert->SelectedIndex = 0;
}

System::Void frmConfig::AdjustCXDropDownWidth(ComboBox^ CX) {
	System::Drawing::Graphics^ ds = CX->CreateGraphics();
	float maxwidth = 0.0;
	for (int i = 0; i < CX->Items->Count; i++)
		maxwidth = max(maxwidth, ds->MeasureString(CX->Items[i]->ToString(), CX->Font).Width);
	CX->DropDownWidth = (int)(maxwidth + 0.5);
	delete ds;
}

System::Void frmConfig::InitCXCmdExInsert() {
	fcgCXCmdExInsert->BeginUpdate();
	fcgCXCmdExInsert->Items->Clear();
	fcgCXCmdExInsert->Items->Add(L"文字列を挿入...");
	fcgCXCmdExInsert->Items->Add(L"ファイル フルパス...");
	System::Drawing::Graphics^ ds = fcgCXCmdExInsert->CreateGraphics();
	float max_width_of_string = 0;
	for (int i = 0; REPLACE_STRINGS_LIST[i].desc; i++)
		max_width_of_string = max(max_width_of_string, ds->MeasureString(String(REPLACE_STRINGS_LIST[i].string).ToString() + L" … ", fcgCXCmdExInsert->Font).Width);
	for (int i = 0; REPLACE_STRINGS_LIST[i].desc; i++) {
		String^ AppenStr = String(REPLACE_STRINGS_LIST[i].string).ToString();
		const int length_of_string = AppenStr->Length;
		AppenStr += L" … ";
		for (float current_width = 0.0; current_width < max_width_of_string; AppenStr = AppenStr->Insert(length_of_string, L" "))
			current_width = ds->MeasureString(AppenStr, fcgCXCmdExInsert->Font).Width;
		AppenStr += String(REPLACE_STRINGS_LIST[i].desc).ToString();
		fcgCXCmdExInsert->Items->Add(AppenStr);
	}
	delete ds;
	fcgCXCmdExInsert->SelectedIndex = 0;
	AdjustCXDropDownWidth(fcgCXCmdExInsert);
	fcgCXCmdExInsert->EndUpdate();
}

/////////////    音声設定関連の関数    ///////////////
System::Void frmConfig::fcgCBAudio2pass_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	if (fcgCBAudio2pass->Checked) {
		fcgCBAudioUsePipe->Checked = false;
		fcgCBAudioUsePipe->Enabled = false;
	} else if (CurrentPipeEnabled) {
		fcgCBAudioUsePipe->Checked = true;
		fcgCBAudioUsePipe->Enabled = true;
	}
}

System::Void frmConfig::fcgCXAudioEncoder_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	setAudioDisplay();
}

System::Void frmConfig::fcgCXAudioEncMode_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	AudioEncodeModeChanged();
}

System::Int32 frmConfig::GetCurrentAudioDefaultBitrate() {
	return sys_dat->exstg->s_aud[fcgCXAudioEncoder->SelectedIndex].mode[fcgCXAudioEncMode->SelectedIndex].bitrate_default;
}

System::Void frmConfig::setAudioDisplay() {
	AUDIO_SETTINGS *astg = &sys_dat->exstg->s_aud[fcgCXAudioEncoder->SelectedIndex];
	//～の指定
	if (str_has_char(astg->filename)) {
		fcgLBAudioEncoderPath->Text = String(astg->filename).ToString() + L" の指定";
		fcgTXAudioEncoderPath->Enabled = true;
		fcgTXAudioEncoderPath->Text = LocalStg.audEncPath[fcgCXAudioEncoder->SelectedIndex];
		fcgBTAudioEncoderPath->Enabled = true;
	} else {
		//filename空文字列(wav出力時)
		fcgLBAudioEncoderPath->Text = L"";
		fcgTXAudioEncoderPath->Enabled = false;
		fcgTXAudioEncoderPath->Text = L"";
		fcgBTAudioEncoderPath->Enabled = false;
	}
	fcgTXAudioEncoderPath->SelectionStart = fcgTXAudioEncoderPath->Text->Length;
	fcgCXAudioEncMode->BeginUpdate();
	fcgCXAudioEncMode->Items->Clear();
	for (int i = 0; i < astg->mode_count; i++)
		fcgCXAudioEncMode->Items->Add(String(astg->mode[i].name).ToString());
	fcgCXAudioEncMode->EndUpdate();
	bool pipe_enabled = (astg->pipe_input && (!(fcgCBAudio2pass->Checked && astg->mode[fcgCXAudioEncMode->SelectedIndex].enc_2pass != 0)));
	CurrentPipeEnabled = pipe_enabled;
	fcgCBAudioUsePipe->Enabled = pipe_enabled;
	fcgCBAudioUsePipe->Checked = pipe_enabled;
	if (fcgCXAudioEncMode->Items->Count > 0)
		fcgCXAudioEncMode->SelectedIndex = 0;
}

System::Void frmConfig::AudioEncodeModeChanged() {
	int index = fcgCXAudioEncMode->SelectedIndex;
	AUDIO_SETTINGS *astg = &sys_dat->exstg->s_aud[fcgCXAudioEncoder->SelectedIndex];
	if (astg->mode[index].bitrate) {
		fcgCXAudioEncMode->Width = fcgCXAudioEncModeSmallWidth;
		fcgLBAudioBitrate->Visible = true;
		fcgNUAudioBitrate->Visible = true;
		fcgNUAudioBitrate->Minimum = astg->mode[index].bitrate_min;
		fcgNUAudioBitrate->Maximum = astg->mode[index].bitrate_max;
		fcgNUAudioBitrate->Increment = astg->mode[index].bitrate_step;
		SetNUValue(fcgNUAudioBitrate, (conf->aud.bitrate != 0) ? conf->aud.bitrate : astg->mode[index].bitrate_default);
	} else {
		fcgCXAudioEncMode->Width = fcgCXAudioEncModeLargeWidth;
		fcgLBAudioBitrate->Visible = false;
		fcgNUAudioBitrate->Visible = false;
		fcgNUAudioBitrate->Minimum = 0;
		fcgNUAudioBitrate->Maximum = 1536; //音声の最大レートは1536kbps
	}
	fcgCBAudio2pass->Enabled = astg->mode[index].enc_2pass != 0;
	if (!fcgCBAudio2pass->Enabled) fcgCBAudio2pass->Checked = false;
	SetfbcBTABEnable(fcgNUAudioBitrate->Visible, (int)fcgNUAudioBitrate->Maximum);
	
	bool delay_cut_available = astg->mode[index].delay > 0;
	fcgLBAudioDelayCut->Visible = delay_cut_available;
	fcgCXAudioDelayCut->Visible = delay_cut_available;
	if (delay_cut_available) {
		const bool delay_cut_edts_available = str_has_char(astg->cmd_raw) && str_has_char(sys_dat->exstg->s_mux[MUXER_MP4_RAW].delay_cmd);
		const int current_idx = fcgCXAudioDelayCut->SelectedIndex;
		const int items_to_set = _countof(AUDIO_DELAY_CUT_MODE) - 1 - ((delay_cut_edts_available) ? 0 : 1);
		fcgCXAudioDelayCut->BeginUpdate();
		fcgCXAudioDelayCut->Items->Clear();
		for (int i = 0; i < items_to_set; i++)
			fcgCXAudioDelayCut->Items->Add(String(AUDIO_DELAY_CUT_MODE[i]).ToString());
		fcgCXAudioDelayCut->EndUpdate();
		fcgCXAudioDelayCut->SelectedIndex = (current_idx >= items_to_set) ? 0 : current_idx;
	} else {
		fcgCXAudioDelayCut->SelectedIndex = 0;
	}
}

///////////////   設定ファイル関連   //////////////////////
System::Void frmConfig::CheckTSItemsEnabled(CONF_GUIEX *current_conf) {
	bool selected = (CheckedStgMenuItem != nullptr);
	fcgTSBSave->Enabled = (selected && memcmp(cnf_stgSelected, current_conf, sizeof(CONF_GUIEX)));
	fcgTSBDelete->Enabled = selected;
}

System::Void frmConfig::UncheckAllDropDownItem(ToolStripItem^ mItem) {
	ToolStripDropDownItem^ DropDownItem = dynamic_cast<ToolStripDropDownItem^>(mItem);
	if (DropDownItem == nullptr)
		return;
	for (int i = 0; i < DropDownItem->DropDownItems->Count; i++) {
		UncheckAllDropDownItem(DropDownItem->DropDownItems[i]);
		ToolStripMenuItem^ item = dynamic_cast<ToolStripMenuItem^>(DropDownItem->DropDownItems[i]);
		if (item != nullptr)
			item->Checked = false;
	}
}

System::Void frmConfig::CheckTSSettingsDropDownItem(ToolStripMenuItem^ mItem) {
	UncheckAllDropDownItem(fcgTSSettings);
	CheckedStgMenuItem = mItem;
	fcgTSSettings->Text = (mItem == nullptr) ? L"プロファイル" : mItem->Text;
	if (mItem != nullptr)
		mItem->Checked = true;
	fcgTSBSave->Enabled = false;
	fcgTSBDelete->Enabled = (mItem != nullptr);
}

ToolStripMenuItem^ frmConfig::fcgTSSettingsSearchItem(String^ stgPath, ToolStripItem^ mItem) {
	if (stgPath == nullptr)
		return nullptr;
	ToolStripDropDownItem^ DropDownItem = dynamic_cast<ToolStripDropDownItem^>(mItem);
	if (DropDownItem == nullptr)
		return nullptr;
	for (int i = 0; i < DropDownItem->DropDownItems->Count; i++) {
		ToolStripMenuItem^ item = fcgTSSettingsSearchItem(stgPath, DropDownItem->DropDownItems[i]);
		if (item != nullptr)
			return item;
		item = dynamic_cast<ToolStripMenuItem^>(DropDownItem->DropDownItems[i]);
		if (item      != nullptr && 
			item->Tag != nullptr && 
			0 == String::Compare(item->Tag->ToString(), stgPath, true))
			return item;
	}
	return nullptr;
}

ToolStripMenuItem^ frmConfig::fcgTSSettingsSearchItem(String^ stgPath) {
	return fcgTSSettingsSearchItem((stgPath != nullptr && stgPath->Length > 0) ? Path::GetFullPath(stgPath) : nullptr, fcgTSSettings);
}

System::Void frmConfig::SaveToStgFile(String^ stgName) {
	size_t nameLen = CountStringBytes(stgName) + 1; 
	char *stg_name = (char *)malloc(nameLen);
	GetCHARfromString(stg_name, nameLen, stgName);
	init_CONF_GUIEX(cnf_stgSelected, fcgCBUsehighbit->Checked);
	FrmToConf(cnf_stgSelected);
	String^ stgDir = Path::GetDirectoryName(stgName);
	if (!Directory::Exists(stgDir))
		Directory::CreateDirectory(stgDir);
	guiEx_config exCnf;
	int result = exCnf.save_guiEx_conf(cnf_stgSelected, stg_name);
	free(stg_name);
	switch (result) {
		case CONF_ERROR_FILE_OPEN:
			MessageBox::Show(L"設定ファイルオープンに失敗しました。", L"エラー", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		case CONF_ERROR_INVALID_FILENAME:
			MessageBox::Show(L"ファイル名に使用できない文字が含まれています。\n保存できません。", L"エラー", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		default:
			break;
	}
	init_CONF_GUIEX(cnf_stgSelected, fcgCBUsehighbit->Checked);
	FrmToConf(cnf_stgSelected);
}

System::Void frmConfig::fcgTSBSave_Click(System::Object^  sender, System::EventArgs^  e) {
	if (CheckedStgMenuItem != nullptr)
		SaveToStgFile(CheckedStgMenuItem->Tag->ToString());
	CheckTSSettingsDropDownItem(CheckedStgMenuItem);
}

System::Void frmConfig::fcgTSBSaveNew_Click(System::Object^  sender, System::EventArgs^  e) {
	frmSaveNewStg::Instance::get()->setStgDir(String(sys_dat->exstg->s_local.stg_dir).ToString());
	if (CheckedStgMenuItem != nullptr)
		frmSaveNewStg::Instance::get()->setFilename(CheckedStgMenuItem->Text);
	frmSaveNewStg::Instance::get()->ShowDialog();
	String^ stgName = frmSaveNewStg::Instance::get()->StgFileName;
	if (stgName != nullptr && stgName->Length)
		SaveToStgFile(stgName);
	RebuildStgFileDropDown(nullptr);
	CheckTSSettingsDropDownItem(fcgTSSettingsSearchItem(stgName));
}

System::Void frmConfig::DeleteStgFile(ToolStripMenuItem^ mItem) {
	if (System::Windows::Forms::DialogResult::OK ==
		MessageBox::Show(L"設定ファイル " + mItem->Text + L" を削除してよろしいですか?",
		L"エラー", MessageBoxButtons::OKCancel, MessageBoxIcon::Exclamation)) 
	{
		File::Delete(mItem->Tag->ToString());
		RebuildStgFileDropDown(nullptr);
		CheckTSSettingsDropDownItem(nullptr);
		SetfcgTSLSettingsNotes(L"");
	}
}

System::Void frmConfig::fcgTSBDelete_Click(System::Object^  sender, System::EventArgs^  e) {
	DeleteStgFile(CheckedStgMenuItem);
}

System::Void frmConfig::fcgTSSettings_DropDownItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
	ToolStripMenuItem^ ClickedMenuItem = dynamic_cast<ToolStripMenuItem^>(e->ClickedItem);
	if (ClickedMenuItem == nullptr)
		return;
	if (ClickedMenuItem->Tag == nullptr || ClickedMenuItem->Tag->ToString()->Length == 0)
		return;
	CONF_GUIEX load_stg;
	guiEx_config exCnf;
	char stg_path[MAX_PATH_LEN];
	GetCHARfromString(stg_path, sizeof(stg_path), ClickedMenuItem->Tag->ToString());
	if (exCnf.load_guiEx_conf(&load_stg, stg_path) == CONF_ERROR_FILE_OPEN) {
		if (MessageBox::Show(L"設定ファイルオープンに失敗しました。\n"
			               + L"このファイルを削除しますか?",
						   L"エラー", MessageBoxButtons::YesNo, MessageBoxIcon::Error)
						   == System::Windows::Forms::DialogResult::Yes)
			DeleteStgFile(ClickedMenuItem);
		return;
	}
	ConfToFrm(&load_stg, true);
	CheckTSSettingsDropDownItem(ClickedMenuItem);
	memcpy(cnf_stgSelected, &load_stg, sizeof(CONF_GUIEX));
}

System::Void frmConfig::RebuildStgFileDropDown(ToolStripDropDownItem^ TS, String^ dir) {
	array<String^>^ subDirs = Directory::GetDirectories(dir);
	for (int i = 0; i < subDirs->Length; i++) {
		ToolStripMenuItem^ DDItem = gcnew ToolStripMenuItem(L"[ " + subDirs[i]->Substring(dir->Length+1) + L" ]");
		DDItem->DropDownItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &frmConfig::fcgTSSettings_DropDownItemClicked);
		DDItem->ForeColor = Color::Blue;
		DDItem->Tag = nullptr;
		RebuildStgFileDropDown(DDItem, subDirs[i]);
		TS->DropDownItems->Add(DDItem);
	}
	array<String^>^ stgList = Directory::GetFiles(dir, L"*.stg");
	for (int i = 0; i < stgList->Length; i++) {
		ToolStripMenuItem^ mItem = gcnew ToolStripMenuItem(Path::GetFileNameWithoutExtension(stgList[i]));
		mItem->Tag = stgList[i];
		TS->DropDownItems->Add(mItem);
	}
}

System::Void frmConfig::RebuildStgFileDropDown(String^ stgDir) {
	fcgTSSettings->DropDownItems->Clear();
	if (stgDir != nullptr)
		CurrentStgDir = stgDir;
	if (!Directory::Exists(CurrentStgDir))
		Directory::CreateDirectory(CurrentStgDir);
	RebuildStgFileDropDown(fcgTSSettings, Path::GetFullPath(CurrentStgDir));
}

//////////////   初期化関連     ////////////////
System::Void frmConfig::InitData(CONF_GUIEX *set_config, const SYSTEM_DATA *system_data) {
	if (set_config->size_all != CONF_INITIALIZED) {
		//初期化されていなければ初期化する
		init_CONF_GUIEX(set_config, FALSE);
	}
	conf = set_config;
	sys_dat = system_data;
}

System::Void frmConfig::InitComboBox() {
	//コンボボックスに値を設定する
	setComboBox(fcgCXAudioTempDir,   audtempdir_desc);
	setComboBox(fcgCXMP4BoxTempDir,  mp4boxtempdir_desc);
	setComboBox(fcgCXTempDir,        tempdir_desc);
	
	//x265
	setComboBox(fcgCXCSP,            list_output_csp_x265);
	setComboBox(fcgCXAQMode,         list_aq);
	setComboBox(fcgCXAspectRatio,    aspect_desc);
	setComboBox(fcgCXX265Mode,       x265_encodemode_desc);
	setComboBox(fcgCXME,             list_me_x265);
	setComboBox(fcgCXSubME,          list_subme_x265);
	setComboBox(fcgCXBadapt,         list_b_adpat);
	setComboBox(fcgCXPreset,         sys_dat->exstg->s_x265.preset.name);
	setComboBox(fcgCXProfile,        sys_dat->exstg->s_x265.profile.name);
	setComboBox(fcgCXTune,		     sys_dat->exstg->s_x265.tune.name);
	setComboBox(fcgCXInterlaced,     interlaced_desc);
	setComboBox(fcgCXTransfer,       list_transfer);
	setComboBox(fcgCXColorMatrix,    list_colormatrix);
	setComboBox(fcgCXColorPrim,      list_colorprim);
	setComboBox(fcgCXVideoFormat,    list_videoformat);

	setComboBox(fcgCXAudioEncTiming, audio_enc_timing_desc);
	setComboBox(fcgCXAudioDelayCut,  AUDIO_DELAY_CUT_MODE);

	InitCXCmdExInsert();

	setMuxerCmdExNames(fcgCXMP4CmdEx, MUXER_MP4);
	setMuxerCmdExNames(fcgCXMKVCmdEx, MUXER_MKV);
	setMuxerCmdExNames(fcgCXMPGCmdEx, MUXER_MPG);

	setAudioEncoderNames();

	setPriorityList(fcgCXX264Priority);
	setPriorityList(fcgCXMuxPriority);
	setPriorityList(fcgCXAudioPriority);
}

System::Void frmConfig::SetTXMaxLen(TextBox^ TX, int max_len) {
	TX->MaxLength = max_len;
	TX->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmConfig::TX_LimitbyBytes);
}

System::Void frmConfig::SetTXMaxLenAll() {
	//MaxLengthに最大文字数をセットし、それをもとにバイト数計算を行うイベントをセットする。
	SetTXMaxLen(fcgTXCmdEx,                sizeof(conf->vid.cmdex) - 1);
	SetTXMaxLen(fcgTXX265Path,             sizeof(sys_dat->exstg->s_x265.fullpath) - 1);
	SetTXMaxLen(fcgTXX265PathSub,          sizeof(sys_dat->exstg->s_x265.fullpath) - 1);
	SetTXMaxLen(fcgTXX265PathSubhighbit,   sizeof(sys_dat->exstg->s_x265.fullpath_highbit) - 1);
	SetTXMaxLen(fcgTXAudioEncoderPath,     sizeof(sys_dat->exstg->s_aud[0].fullpath) - 1);
	SetTXMaxLen(fcgTXMP4MuxerPath,         sizeof(sys_dat->exstg->s_mux[MUXER_MP4].fullpath) - 1);
	SetTXMaxLen(fcgTXMKVMuxerPath,         sizeof(sys_dat->exstg->s_mux[MUXER_MKV].fullpath) - 1);
	SetTXMaxLen(fcgTXTC2MP4Path,           sizeof(sys_dat->exstg->s_mux[MUXER_TC2MP4].fullpath) - 1);
	SetTXMaxLen(fcgTXMPGMuxerPath,         sizeof(sys_dat->exstg->s_mux[MUXER_MPG].fullpath) - 1);
	SetTXMaxLen(fcgTXMP4RawPath,           sizeof(sys_dat->exstg->s_mux[MUXER_MP4_RAW].fullpath) - 1);
	SetTXMaxLen(fcgTXCustomTempDir,        sizeof(sys_dat->exstg->s_local.custom_tmp_dir) - 1);
	SetTXMaxLen(fcgTXCustomAudioTempDir,   sizeof(sys_dat->exstg->s_local.custom_audio_tmp_dir) - 1);
	SetTXMaxLen(fcgTXMP4BoxTempDir,        sizeof(sys_dat->exstg->s_local.custom_mp4box_tmp_dir) - 1);
	SetTXMaxLen(fcgTXStatusFile,           sizeof(conf->vid.stats) - 1);
	SetTXMaxLen(fcgTXBatBeforePath,        sizeof(conf->oth.batfile_before) - 1);
	SetTXMaxLen(fcgTXBatAfterPath,         sizeof(conf->oth.batfile_after) - 1);

	fcgTSTSettingsNotes->MaxLength     =   sizeof(conf->oth.notes) - 1;
}

System::Void frmConfig::InitStgFileList() {
	RebuildStgFileDropDown(String(sys_dat->exstg->s_local.stg_dir).ToString());
	stgChanged = false;
	CheckTSSettingsDropDownItem(nullptr);
}

System::Void frmConfig::fcgChangeEnabled(System::Object^  sender, System::EventArgs^  e) {
	fcgLBX265PathSub->Visible = fcgTSBCMDOnly->Checked;
	fcgLBX265PathSub8bit->Visible = fcgTSBCMDOnly->Checked;
	fcgLBX265PathSubhighbit->Visible = fcgTSBCMDOnly->Checked;
	fcgTXX265PathSub->Visible = fcgTSBCMDOnly->Checked;
	fcgBTX265PathSub->Visible = fcgTSBCMDOnly->Checked;
	fcgTXX265PathSubhighbit->Visible = fcgTSBCMDOnly->Checked;
	fcgBTX265PathSubhighbit->Visible = fcgTSBCMDOnly->Checked;
	fcgBTCmdEx->Visible = !fcgTSBCMDOnly->Checked;
	fcgCBNulOutCLI->Visible = fcgTSBCMDOnly->Checked;
}

System::Void frmConfig::fcgChangeMuxerVisible(System::Object^  sender, System::EventArgs^  e) {
	//tc2mp4のチェック
	const bool enable_tc2mp4_muxer = (0 != str_has_char(sys_dat->exstg->s_mux[MUXER_TC2MP4].base_cmd));
	fcgTXTC2MP4Path->Visible = enable_tc2mp4_muxer;
	fcgLBTC2MP4Path->Visible = enable_tc2mp4_muxer;
	fcgBTTC2MP4Path->Visible = enable_tc2mp4_muxer;
	//mp4 rawのチェック
	const bool enable_mp4raw_muxer = (0 != str_has_char(sys_dat->exstg->s_mux[MUXER_MP4_RAW].base_cmd));
	fcgTXMP4RawPath->Visible = enable_mp4raw_muxer;
	fcgLBMP4RawPath->Visible = enable_mp4raw_muxer;
	fcgBTMP4RawPath->Visible = enable_mp4raw_muxer;
	//一時フォルダのチェック
	const bool enable_mp4_tmp = (0 != str_has_char(sys_dat->exstg->s_mux[MUXER_MP4].tmp_cmd));
	fcgCXMP4BoxTempDir->Visible = enable_mp4_tmp;
	fcgLBMP4BoxTempDir->Visible = enable_mp4_tmp;
	fcgTXMP4BoxTempDir->Visible = enable_mp4_tmp;
	fcgBTMP4BoxTempDir->Visible = enable_mp4_tmp;
	//Apple Chapterのチェック
	bool enable_mp4_apple_cmdex = false;
	for (int i = 0; i < sys_dat->exstg->s_mux[MUXER_MP4].ex_count; i++)
		enable_mp4_apple_cmdex |= (0 != str_has_char(sys_dat->exstg->s_mux[MUXER_MP4].ex_cmd[i].cmd_apple));
	fcgCBMP4MuxApple->Visible = enable_mp4_apple_cmdex;

	//位置の調整
	static const int HEIGHT = 31;
	fcgLBTC2MP4Path->Location = Point(fcgLBTC2MP4Path->Location.X, fcgLBMP4MuxerPath->Location.Y + HEIGHT * enable_tc2mp4_muxer);
	fcgTXTC2MP4Path->Location = Point(fcgTXTC2MP4Path->Location.X, fcgTXMP4MuxerPath->Location.Y + HEIGHT * enable_tc2mp4_muxer);
	fcgBTTC2MP4Path->Location = Point(fcgBTTC2MP4Path->Location.X, fcgBTMP4MuxerPath->Location.Y + HEIGHT * enable_tc2mp4_muxer);
	fcgLBMP4RawPath->Location = Point(fcgLBMP4RawPath->Location.X, fcgLBTC2MP4Path->Location.Y   + HEIGHT * enable_mp4raw_muxer);
	fcgTXMP4RawPath->Location = Point(fcgTXMP4RawPath->Location.X, fcgTXTC2MP4Path->Location.Y   + HEIGHT * enable_mp4raw_muxer);
	fcgBTMP4RawPath->Location = Point(fcgBTMP4RawPath->Location.X, fcgBTTC2MP4Path->Location.Y   + HEIGHT * enable_mp4raw_muxer);
}

System::Void frmConfig::SetStgEscKey(bool Enable) {
	if (this->KeyPreview == Enable)
		return;
	this->KeyPreview = Enable;
	if (Enable)
		this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &frmConfig::frmConfig_KeyDown);
	else
		this->KeyDown -= gcnew System::Windows::Forms::KeyEventHandler(this, &frmConfig::frmConfig_KeyDown);
}

System::Void frmConfig::AdjustLocation() {
	//デスクトップ領域(タスクバー等除く)
	System::Drawing::Rectangle screen = System::Windows::Forms::Screen::GetWorkingArea(this);
	//現在のデスクトップ領域の座標
	Point CurrentDesktopLocation = this->DesktopLocation::get();
	//チェック開始
	bool ChangeLocation = false;
	if (CurrentDesktopLocation.X + this->Size.Width > screen.Width) {
		ChangeLocation = true;
		CurrentDesktopLocation.X = clamp(screen.X - this->Size.Width, 4, CurrentDesktopLocation.X);
	}
	if (CurrentDesktopLocation.Y + this->Size.Height > screen.Height) {
		ChangeLocation = true;
		CurrentDesktopLocation.Y = clamp(screen.Y - this->Size.Height, 4, CurrentDesktopLocation.Y);
	}
	if (ChangeLocation) {
		this->StartPosition = FormStartPosition::Manual;
		this->DesktopLocation::set(CurrentDesktopLocation);
	}
}

System::Void frmConfig::initUpdater() {
#if ENABLE_AUOSETUP
	frmExeUpdate = gcnew frmUpdate();
	frmExeUpdate->Owner = this;
	frmExeUpdate->init(gcnew x265guiEx::DelegateProcessFin(this, &frmConfig::fcgTSBUpdate_CheckFinished),
		               gcnew x265guiEx::DelegateProcessFin(this, &frmConfig::fcgTSBUpdate_UpdateFinished));
	if (str_has_char(sys_dat->exstg->s_local.last_update_check)) {
		try {
			DateTime dtLastUpdate = DateTime::Parse(String(sys_dat->exstg->s_local.last_update_check).ToString());
			TimeSpan tsFromLastUpdate = DateTime::Now - dtLastUpdate;
			//一週間以内に確認していれば自動的に更新チェック
			if (tsFromLastUpdate < TimeSpan(7, 0, 0, 0)) {
				return;
			}
		} catch (...) {
			//そのまま下へ
		}
	}
	frmExeUpdate->startCheck();
#else
	fcgTSBUpdate->Enabled = false;
#endif
}

System::Void frmConfig::InitForm() {
	initUpdater();
	//fcgTabPageCtrlの初期化
	//fcgTabPageVideoController = gcnew TabPageCtrl(fcgtabControlVideo);
	//ローカル設定のロード
	LoadLocalStg();
	//ローカル設定の反映
	SetLocalStg();
	//設定ファイル集の初期化
	InitStgFileList();
	//コンボボックスの値を設定
	InitComboBox();
	//タイムコードのappendix(後付修飾子)を反映
	fcgCBAuoTcfileout->Text = L"タイムコード出力 (" + String(sys_dat->exstg->s_append.tc).ToString() + L")";
	//タイトル表示
	this->Text = String(AUO_FULL_NAME).ToString();
	//バージョン情報,コンパイル日時
	fcgLBVersion->Text     = String(AUO_VERSION_NAME).ToString();
	fcgLBVersionDate->Text = L"build " + String(__DATE__).ToString() + L" " + String(__TIME__).ToString();
	//タイマーの初期化
	InitTimer();
	//ツールチップ
	SetHelpToolTips();
	SetX265VersionToolTip(LocalStg.x265Path, false);
	SetX265VersionToolTip(LocalStg.x265Pathhighbit, true);
	ActivateToolTip(sys_dat->exstg->s_local.disable_tooltip_help == FALSE);
	//パラメータセット
	ConfToFrm(conf, true);
	//イベントセット
	SetTXMaxLenAll(); //テキストボックスの最大文字数
	SetAllCheckChangedEvents(this); //変更の確認,ついでにNUのフォームの変更可不可を更新
	fcgChangeMuxerVisible(nullptr, nullptr);
	fcgChangeEnabled(nullptr, nullptr);
	fcgCBAFS_CheckedChanged(nullptr, nullptr);
	EnableSettingsNoteChange(false);
	//コマンドラインの更新
	fcgRebuildCmd(nullptr, nullptr);
	//表示位置の調整
	AdjustLocation();
	AddfcgLBAMPAutoBitrate();
	AddCheckAMPAutoBitrateEvent();
	this->fcgTSBCMDOnly->CheckedChanged += gcnew System::EventHandler(this, &frmConfig::fcgTSBRearrageTabs_CheckedChanged);
	fcgTSBRearrageTabs_CheckedChanged(nullptr, nullptr);
	//キー設定
	SetStgEscKey(sys_dat->exstg->s_local.enable_stg_esc_key != 0);
	//フォントの設定
	if (str_has_char(sys_dat->exstg->s_local.conf_font.name))
		SetFontFamilyToForm(this, gcnew FontFamily(String(sys_dat->exstg->s_local.conf_font.name).ToString()), this->Font->FontFamily);
}

/////////////         データ <-> GUI     /////////////
System::Void frmConfig::ConfToFrm(CONF_GUIEX *cnf, bool all) {
	{
	//x265
	CONF_X265 *cx265 = &cnf->x265;
	memcpy(cnf_fcgTemp, cx265, sizeof(CONF_X265)); //一時保存用
	fcgCBUsehighbit->Checked = 8 < cx265->bit_depth;
	switch (cx265->rc_mode) {
		case X265_RC_QP:
			fcgCXX265Mode->SelectedIndex = 1;
			break;
		case X265_RC_BITRATE:
			if (cx265->use_auto_npass)
				fcgCXX265Mode->SelectedIndex = 5;
			else {
				switch (cx265->pass) {
					case 0:  fcgCXX265Mode->SelectedIndex = 0; break;
					case 1:  fcgCXX265Mode->SelectedIndex = 3; break;
					default: fcgCXX265Mode->SelectedIndex = 4; break;
				}
			}
			break;
		case X265_RC_CRF:
		default:
			fcgCXX265Mode->SelectedIndex = (cx265->use_auto_npass) ? 6 : 2;
			break;
	}
	fcgCXX265Mode_SelectedIndexChanged(nullptr, nullptr); //こいつをやっとかないと更新されないこともある
	fcgCBAMPLimitBitrate->Checked  = (cnf->vid.amp_check & AMPLIMIT_BITRATE) != 0;
	fcgCBAMPLimitFileSize->Checked = (cnf->vid.amp_check & AMPLIMIT_FILE_SIZE) != 0;
	SetNUValue(fcgNUAMPLimitFileSize, cnf->vid.amp_limit_file_size);
	SetNUValue(fcgNUAMPLimitBitrate,  cnf->vid.amp_limit_bitrate);

	SetCXIndex(fcgCXPreset,           cx265->preset);
	SetCXIndex(fcgCXTune,             cx265->tune);
	SetCXIndex(fcgCXProfile,          cx265->profile);
	
	if (cx265->sar.x * cx265->sar.y < 0)
		cx265->sar.x = cx265->sar.y = 0;
	fcgCXAspectRatio->SelectedIndex= (cx265->sar.x < 0);
	SetNUValue(fcgNUAspectRatioX, abs(cx265->sar.x));
	SetNUValue(fcgNUAspectRatioY, abs(cx265->sar.y));

	SetCXIndex(fcgCXInterlaced,       cx265->interlaced);
	
	SetCXIndex(fcgCXVideoFormat,      cx265->videoformat);
	SetCXIndex(fcgCXCSP,              x265_outputcsp_map[clamp(cx265->output_csp, 0, _countof(x265_outputcsp_map) - 1)]);
	
	SetCXIndex(fcgCXColorMatrix,      cx265->colormatrix);
	SetCXIndex(fcgCXColorPrim,        cx265->colorprim);
	SetCXIndex(fcgCXTransfer,         cx265->transfer);
	fcgCBFullRange->Checked         = cx265->input_range != 0;

	SetNUValue(fcgNUScenecut,         cx265->scenecut);
	SetNUValue(fcgNUKeyintMin,        cx265->keyint_min);
	SetNUValue(fcgNUKeyintMax,        cx265->keyint_max);
	fcgCBOpenGOP->Checked           = cx265->open_gop != 0;
	SetNUValue(fcgNURCLookahead,      cx265->rc_lookahead);
	SetNUValue(fcgNURef,              cx265->ref_frames);
	SetNUValue(fcgNUBframes,          cx265->bframes);
	SetCXIndex(fcgCXBadapt,           cx265->b_adapt);
	fcgCBBpyramid->Checked          = cx265->b_pyramid != 0;
	fcgCBWeightB->Checked           = cx265->weight_b != 0;
	fcgCBWeightP->Checked           = cx265->weight_p != 0;
	
	SetNUValue(fcgNUVBVbuf,           cx265->vbv_bufsize);
	SetNUValue(fcgNUVBVmax,           cx265->vbv_maxrate);

	SetNUValue(fcgNURD,               cx265->rd);
	SetCXIndex(fcgCXAQMode,           cx265->aq_mode);
	SetNUValue(fcgNUAQStrength,       cx265->aq_strength);
	SetNUValue(fcgNUPsyRD,            cx265->psy_rd.x);
	SetNUValue(fcgNUPsyRDOQ,          cx265->psy_rdoq);
	fcgCBCUTree->Checked            = cx265->cutree != 0;

	SetNUValue(fcgNUCtu,              cx265->ctu);
	SetNUValue(fcgNUTuIntraDepth,     cx265->tu_intra_depth);
	SetNUValue(fcgNUTuInterDepth,     cx265->tu_inter_depth);
	fcgCBWpp->Checked               = cx265->wpp != 0;
	
	SetCXIndex(fcgCXME,               cx265->me);
	SetCXIndex(fcgCXSubME,            cx265->subme);
	SetNUValue(fcgNUMERange,          cx265->me_range);
	SetNUValue(fcgNUMaxMerge,         cx265->max_merge);
	fcgCBRectMP->Checked            = cx265->rect_mp != 0;
	fcgCBAsymmetricMP->Checked      = cx265->asymmnteric_mp != 0;

	SetNUValue(fcgNUThreads,          cx265->threads);
	SetNUValue(fcgNUFrameThreads,     cx265->frame_threads);

	fcgCBLoopFilter->Checked        = cx265->loop_filter != 0;
	fcgCBSAO->Checked               = cx265->sao != 0;
	}

	 
	if (all) {
		//動画部
		fcgTXStatusFile->Text = (str_has_char(cnf->vid.stats))     ? String(cnf->vid.stats).ToString() : String(DefaultStatusFilePath).ToString();

		fcgCBAFS->Checked                  = cnf->vid.afs != 0;
		fcgCBAFSBitrateCorrection->Checked = cnf->vid.afs_bitrate_correction != 0;
		fcgCBAuoTcfileout->Checked         = cnf->vid.auo_tcfile_out != 0;
		fcgCBCheckKeyframes->Checked       =(cnf->vid.check_keyframe & CHECK_KEYFRAME_AVIUTL) != 0;
		fcgCBSetKeyframeAtChapter->Checked =(cnf->vid.check_keyframe & CHECK_KEYFRAME_CHAPTER) != 0;
		fcgCBInputAsLW48->Checked          = cnf->vid.input_as_lw48 != 0;

		SetCXIndex(fcgCXX264Priority,        cnf->vid.priority);
		SetCXIndex(fcgCXTempDir,             cnf->oth.temp_dir);

		fcgTXCmdEx->Text            = String(cnf->vid.cmdex).ToString();
		if (cnf->oth.disable_guicmd)
			fcgCBNulOutCLI->Checked        = cnf->x265.nul_out != 0;

		//音声
		fcgCBAudioOnly->Checked            = cnf->oth.out_audio_only != 0;
		fcgCBFAWCheck->Checked             = cnf->aud.faw_check != 0;
		SetCXIndex(fcgCXAudioEncoder,        cnf->aud.encoder);
		fcgCBAudio2pass->Checked           = cnf->aud.use_2pass != 0;
		fcgCBAudioUsePipe->Checked = (CurrentPipeEnabled && !cnf->aud.use_wav);
		SetCXIndex(fcgCXAudioDelayCut,       cnf->aud.delay_cut);
		SetCXIndex(fcgCXAudioEncMode,        cnf->aud.enc_mode);
		SetNUValue(fcgNUAudioBitrate,       (cnf->aud.bitrate != 0) ? cnf->aud.bitrate : GetCurrentAudioDefaultBitrate());
		SetCXIndex(fcgCXAudioPriority,       cnf->aud.priority);
		SetCXIndex(fcgCXAudioTempDir,        cnf->aud.aud_temp_dir);
		SetCXIndex(fcgCXAudioEncTiming,      cnf->aud.audio_encode_timing);

		//mux
		fcgCBMP4MuxerExt->Checked          = cnf->mux.disable_mp4ext == 0;
		fcgCBMP4MuxApple->Checked          = cnf->mux.apple_mode != 0;
		SetCXIndex(fcgCXMP4CmdEx,            cnf->mux.mp4_mode);
		SetCXIndex(fcgCXMP4BoxTempDir,       cnf->mux.mp4_temp_dir);
		fcgCBMKVMuxerExt->Checked          = cnf->mux.disable_mkvext == 0;
		SetCXIndex(fcgCXMKVCmdEx,            cnf->mux.mkv_mode);
		fcgCBMPGMuxerExt->Checked          = cnf->mux.disable_mpgext == 0;
		SetCXIndex(fcgCXMPGCmdEx,            cnf->mux.mpg_mode);
		SetCXIndex(fcgCXMuxPriority,         cnf->mux.priority);

		fcgCBRunBatBefore->Checked         =(cnf->oth.run_bat & RUN_BAT_BEFORE) != 0;
		fcgCBRunBatAfter->Checked          =(cnf->oth.run_bat & RUN_BAT_AFTER) != 0;
		fcgCBWaitForBatBefore->Checked     =(cnf->oth.dont_wait_bat_fin & RUN_BAT_BEFORE) == 0;
		fcgCBWaitForBatAfter->Checked      =(cnf->oth.dont_wait_bat_fin & RUN_BAT_AFTER) == 0;
		fcgTXBatBeforePath->Text           = String(cnf->oth.batfile_before).ToString();
		fcgTXBatAfterPath->Text            = String(cnf->oth.batfile_after).ToString();

		SetfcgTSLSettingsNotes(cnf->oth.notes);

		//cli mode
		fcgTSBCMDOnly->Checked             = cnf->oth.disable_guicmd != 0;
	}

	this->ResumeLayout();
	this->PerformLayout();
}

System::Void frmConfig::FrmToConf(CONF_GUIEX *cnf) {
	//これもひたすら書くだけ。めんどい
	//x265
	cnf->x265.bit_depth            = fcgCBUsehighbit->Checked ? 16 : 8;
	cnf->x265.rc_mode              = cnf_fcgTemp->rc_mode;
	cnf->x265.bitrate              = cnf_fcgTemp->bitrate;
	cnf->x265.qp                   = cnf_fcgTemp->qp;
	cnf->x265.crf                  = cnf_fcgTemp->crf;
	cnf->x265.nul_out              = fcgCBNulOut->Checked;
	cnf->x265.pass                 = cnf_fcgTemp->pass;
	cnf->x265.slow_first_pass      = cnf_fcgTemp->slow_first_pass;
	cnf->x265.use_auto_npass       = cnf_fcgTemp->use_auto_npass;
	cnf->x265.auto_npass           = (int)fcgNUAutoNPass->Value;
	cnf->vid.amp_check             = NULL;
	cnf->vid.amp_check            |= fcgCBAMPLimitBitrate->Checked ? AMPLIMIT_BITRATE : NULL;
	cnf->vid.amp_check            |= fcgCBAMPLimitFileSize->Checked ? AMPLIMIT_FILE_SIZE : NULL;
	cnf->vid.amp_limit_bitrate     = (double)fcgNUAMPLimitBitrate->Value;
	cnf->vid.amp_limit_file_size   = (double)fcgNUAMPLimitFileSize->Value;
	cnf->x265.preset               = fcgCXPreset->SelectedIndex;
	cnf->x265.tune                 = fcgCXTune->SelectedIndex;
	cnf->x265.profile              = fcgCXProfile->SelectedIndex;

	cnf->x265.sar.x                = (int)fcgNUAspectRatioX->Value * ((fcgCXAspectRatio->SelectedIndex != 1) ? 1 : -1);
	cnf->x265.sar.y                = (int)fcgNUAspectRatioY->Value * ((fcgCXAspectRatio->SelectedIndex != 1) ? 1 : -1);
	cnf->x265.interlaced           = fcgCXInterlaced->SelectedIndex;
	
	cnf->x265.videoformat          = fcgCXVideoFormat->SelectedIndex;
	cnf->x265.output_csp           = fcgCXCSP->SelectedIndex;

	cnf->x265.colormatrix          = fcgCXColorMatrix->SelectedIndex;
	cnf->x265.colorprim            = fcgCXColorPrim->SelectedIndex;
	cnf->x265.transfer             = fcgCXTransfer->SelectedIndex;
	cnf->x265.input_range          = fcgCBFullRange->Checked;

	cnf->x265.scenecut             = (int)fcgNUScenecut->Value;
	cnf->x265.keyint_min           = (int)fcgNUKeyintMin->Value;
	cnf->x265.keyint_max           = (int)fcgNUKeyintMax->Value;
	cnf->x265.open_gop             = fcgCBOpenGOP->Checked;
	cnf->x265.rc_lookahead         = (int)fcgNURCLookahead->Value;
	cnf->x265.ref_frames           = (int)fcgNURef->Value;
	cnf->x265.bframes              = (int)fcgNUBframes->Value;
	cnf->x265.b_adapt              = fcgCXBadapt->SelectedIndex;
	cnf->x265.b_pyramid            = fcgCBBpyramid->Checked;
	cnf->x265.weight_b             = fcgCBWeightB->Checked;
	cnf->x265.weight_p             = fcgCBWeightP->Checked;

	cnf->x265.vbv_bufsize          = (int)fcgNUVBVbuf->Value;
	cnf->x265.vbv_maxrate          = (int)fcgNUVBVmax->Value;
	
	cnf->x265.rd                   = (int)fcgNURD->Value;
	cnf->x265.aq_mode              = fcgCXAQMode->SelectedIndex;
	cnf->x265.aq_strength          = (float)fcgNUAQStrength->Value;
	cnf->x265.psy_rd.x             = (float)fcgNUPsyRD->Value;
	cnf->x265.psy_rdoq             = (float)fcgNUPsyRDOQ->Value;
	cnf->x265.cutree               = fcgCBCUTree->Checked;

	cnf->x265.ctu                  = (int)fcgNUCtu->Value;
	cnf->x265.tu_intra_depth       = (int)fcgNUTuIntraDepth->Value;
	cnf->x265.tu_inter_depth       = (int)fcgNUTuInterDepth->Value;
	cnf->x265.wpp                  = fcgCBWpp->Checked;

	cnf->x265.me                   = fcgCXME->SelectedIndex;
	cnf->x265.subme                = fcgCXSubME->SelectedIndex;
	cnf->x265.me_range             = (int)fcgNUMERange->Value;
	cnf->x265.max_merge            = (int)fcgNUMaxMerge->Value;
	cnf->x265.rect_mp              = fcgCBRectMP->Checked;
	cnf->x265.asymmnteric_mp       = fcgCBAsymmetricMP->Checked;
	
	cnf->x265.threads              = (int)fcgNUThreads->Value;
	cnf->x265.frame_threads        = (int)fcgNUFrameThreads->Value;

	cnf->x265.loop_filter          = fcgCBLoopFilter->Checked;
	cnf->x265.sao                  = fcgCBSAO->Checked;

	GetCHARfromString(cnf->vid.stats,     sizeof(cnf->vid.stats), fcgTXStatusFile->Text);

	//拡張部
	cnf->vid.afs                    = fcgCBAFS->Checked;
	cnf->vid.afs_bitrate_correction = fcgCBAFSBitrateCorrection->Checked;
	cnf->vid.auo_tcfile_out         = fcgCBAuoTcfileout->Checked;
	cnf->vid.check_keyframe         = CHECK_KEYFRAME_NONE;
	cnf->vid.check_keyframe        |= (fcgCBCheckKeyframes->Checked)       ? CHECK_KEYFRAME_AVIUTL  : CHECK_KEYFRAME_NONE;
	cnf->vid.check_keyframe        |= (fcgCBSetKeyframeAtChapter->Checked) ? CHECK_KEYFRAME_CHAPTER : CHECK_KEYFRAME_NONE;
	cnf->vid.priority               = fcgCXX264Priority->SelectedIndex;
	cnf->vid.input_as_lw48          = fcgCBInputAsLW48->Checked;
	cnf->oth.temp_dir               = fcgCXTempDir->SelectedIndex;
	GetCHARfromString(cnf->vid.cmdex, sizeof(cnf->vid.cmdex), fcgTXCmdEx->Text);

	//音声部
	cnf->aud.encoder                = fcgCXAudioEncoder->SelectedIndex;
	cnf->oth.out_audio_only         = fcgCBAudioOnly->Checked;
	cnf->aud.faw_check              = fcgCBFAWCheck->Checked;
	cnf->aud.enc_mode               = fcgCXAudioEncMode->SelectedIndex;
	cnf->aud.bitrate                = (int)fcgNUAudioBitrate->Value;
	cnf->aud.use_2pass              = fcgCBAudio2pass->Checked;
	cnf->aud.use_wav                = !fcgCBAudioUsePipe->Checked;
	cnf->aud.delay_cut              = fcgCXAudioDelayCut->SelectedIndex;
	cnf->aud.priority               = fcgCXAudioPriority->SelectedIndex;
	cnf->aud.audio_encode_timing    = fcgCXAudioEncTiming->SelectedIndex;
	cnf->aud.aud_temp_dir           = fcgCXAudioTempDir->SelectedIndex;

	//mux部
	cnf->mux.disable_mp4ext         = !fcgCBMP4MuxerExt->Checked;
	cnf->mux.apple_mode             = fcgCBMP4MuxApple->Checked;
	cnf->mux.mp4_mode               = fcgCXMP4CmdEx->SelectedIndex;
	cnf->mux.mp4_temp_dir           = fcgCXMP4BoxTempDir->SelectedIndex;
	cnf->mux.disable_mkvext         = !fcgCBMKVMuxerExt->Checked;
	cnf->mux.mkv_mode               = fcgCXMKVCmdEx->SelectedIndex;
	cnf->mux.disable_mpgext         = !fcgCBMPGMuxerExt->Checked;
	cnf->mux.mpg_mode               = fcgCXMPGCmdEx->SelectedIndex;
	cnf->mux.priority               = fcgCXMuxPriority->SelectedIndex;

	cnf->oth.run_bat                = RUN_BAT_NONE;
	cnf->oth.run_bat               |= (fcgCBRunBatBefore->Checked) ? RUN_BAT_BEFORE : NULL;
	cnf->oth.run_bat               |= (fcgCBRunBatAfter->Checked) ? RUN_BAT_AFTER : NULL;
	cnf->oth.dont_wait_bat_fin      = RUN_BAT_NONE;
	cnf->oth.dont_wait_bat_fin     |= (!fcgCBWaitForBatBefore->Checked) ? RUN_BAT_BEFORE : NULL;
	cnf->oth.dont_wait_bat_fin     |= (!fcgCBWaitForBatAfter->Checked) ? RUN_BAT_AFTER : NULL;
	GetCHARfromString(cnf->oth.batfile_before, sizeof(cnf->oth.batfile_before), fcgTXBatBeforePath->Text);
	GetCHARfromString(cnf->oth.batfile_after,  sizeof(cnf->oth.batfile_after),  fcgTXBatAfterPath->Text);

	GetfcgTSLSettingsNotes(cnf->oth.notes, sizeof(cnf->oth.notes));

	//cli mode
	cnf->oth.disable_guicmd         = fcgTSBCMDOnly->Checked;
	if (cnf->oth.disable_guicmd) {
		cnf->x265.nul_out           = fcgCBNulOutCLI->Checked;
	}

	//制約条件として適用
	set_profile_to_conf(&cnf->x265, cnf->x265.profile);
}

System::Void frmConfig::GetfcgTSLSettingsNotes(char *notes, int nSize) {
	ZeroMemory(notes, nSize);
	if (fcgTSLSettingsNotes->ForeColor == Color::FromArgb(StgNotesColor[0][0], StgNotesColor[0][1], StgNotesColor[0][2]))
		GetCHARfromString(notes, nSize, fcgTSLSettingsNotes->Text);
}

System::Void frmConfig::SetfcgTSLSettingsNotes(const char *notes) {
	if (str_has_char(notes)) {
		fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[0][0], StgNotesColor[0][1], StgNotesColor[0][2]);
		fcgTSLSettingsNotes->Text = String(notes).ToString();
	} else {
		fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[1][0], StgNotesColor[1][1], StgNotesColor[1][2]);
		fcgTSLSettingsNotes->Text = String(DefaultStgNotes).ToString();
	}
}

System::Void frmConfig::SetfcgTSLSettingsNotes(String^ notes) {
	if (notes->Length && String::Compare(notes, String(DefaultStgNotes).ToString()) != 0) {
		fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[0][0], StgNotesColor[0][1], StgNotesColor[0][2]);
		fcgTSLSettingsNotes->Text = notes;
	} else {
		fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[1][0], StgNotesColor[1][1], StgNotesColor[1][2]);
		fcgTSLSettingsNotes->Text = String(DefaultStgNotes).ToString();
	}
}

System::Void frmConfig::SetChangedEvent(Control^ control, System::EventHandler^ _event) {
	System::Type^ ControlType = control->GetType();
	if (ControlType == NumericUpDown::typeid)
		((NumericUpDown^)control)->ValueChanged += _event;
	else if (ControlType == ComboBox::typeid)
		((ComboBox^)control)->SelectedIndexChanged += _event;
	else if (ControlType == CheckBox::typeid)
		((CheckBox^)control)->CheckedChanged += _event;
	else if (ControlType == TextBox::typeid)
		((TextBox^)control)->TextChanged += _event;
}

System::Void frmConfig::SetToolStripEvents(ToolStrip^ TS, System::Windows::Forms::MouseEventHandler^ _event) {
	for (int i = 0; i < TS->Items->Count; i++) {
		ToolStripButton^ TSB = dynamic_cast<ToolStripButton^>(TS->Items[i]);
		if (TSB != nullptr) TSB->MouseDown += _event;
	}
}

System::Void frmConfig::SetAllCheckChangedEvents(Control ^top) {
	//再帰を使用してすべてのコントロールのtagを調べ、イベントをセットする
	for (int i = 0; i < top->Controls->Count; i++) {
		System::Type^ type = top->Controls[i]->GetType();
		if (type == NumericUpDown::typeid)
			top->Controls[i]->Enter += gcnew System::EventHandler(this, &frmConfig::NUSelectAll);

		if (type == Label::typeid || type == Button::typeid)
			;
		else if (type == ToolStrip::typeid)
			SetToolStripEvents((ToolStrip^)(top->Controls[i]), gcnew System::Windows::Forms::MouseEventHandler(this, &frmConfig::fcgTSItem_MouseDown));
		else if (top->Controls[i]->Tag == nullptr)
			SetAllCheckChangedEvents(top->Controls[i]);
		else if (String::Equals(top->Controls[i]->Tag->ToString(), L"reCmd"))
			SetChangedEvent(top->Controls[i], gcnew System::EventHandler(this, &frmConfig::fcgRebuildCmd));
		else if (String::Equals(top->Controls[i]->Tag->ToString(), L"chValue"))
			SetChangedEvent(top->Controls[i], gcnew System::EventHandler(this, &frmConfig::CheckOtherChanges));
		else
			SetAllCheckChangedEvents(top->Controls[i]);
	}
}

System::Void frmConfig::SetHelpToolTipsColorMatrixX265(Control^ control, const char *type) {
	const X265_OPTION_STR *list = get_option_list_x265(type);
	fcgTTX265->SetToolTip(control,      L"--" + String(type).ToString() + L"\n"
		+ L"auto とするとAviutlの色空間「自動」に合わせ\n"
		+ L"以下のように設定します。\n"
		+ L"縦解像度" + COLOR_MATRIX_THRESHOLD + L"以上 … " + String(list[COLOR_MATRIX_HD].desc).ToString() + L"\n"
		+ L"縦解像度" + COLOR_MATRIX_THRESHOLD + L"未満 … " + String(list[COLOR_MATRIX_SD].desc).ToString() + L"\n"
		+ L"よくわからない場合は 指定なし が無難です。"
		);
}

System::Void frmConfig::SetHelpToolTips() {
	//x265
	fcgTTX265->SetToolTip(fcgCBUsehighbit, L"" 
		+ L"--input-depth 16\n"
		+ L"\n"
		+ L"high bit-depthでエンコードを行います。\n"
		+ L"x265もhigh bit depthのものを使用してください。\n"
		+ L"通常のプレーヤーでは再生できないこともあるため、\n"
		+ L"high bit depthエンコードがなにかを理解している場合にのみ、\n"
		+ L"使用してください。\n"
		+ L"\n"
		+ L"8bit用x265.exeとhigh bit depth用x265.exeは別々に設定でき、\n"
		+ L"このチェックボックスによって切り替わります。"
		);
	fcgTTX265->SetToolTip(fcgBTX265Path, L""
		+ L"x265.exeの場所を指定します。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);
	fcgTTX265->SetToolTip(fcgBTX265PathSub, L""
		+ L"x265.exeの場所を指定します。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);
	fcgTTX265->SetToolTip(fcgBTX265PathSubhighbit, L""
		+ L"x265.exe(high bit depth用)の場所を指定します。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);
	fcgTTX265->SetToolTip(fcgCXX265Mode, L""
		+ L"【シングルパス】\n"
		+ L"   " + String(x265_encodemode_desc[2]).ToString()->Replace(L"シングルパス - ", L"") + L"\t … --crf\n"
		+ L"   " + String(x265_encodemode_desc[1]).ToString()->Replace(L"シングルパス - ", L"") + L"\t\t … --qp\n"
		+ L"   " + String(x265_encodemode_desc[0]).ToString()->Replace(L"シングルパス - ", L"") + L"\t … --bitrate\n"
		+ L"\n"
		+ L"【マルチパス】\n"
		+ L"   " + String(x265_encodemode_desc[3]).ToString()->Replace(L"マルチパス - ", L"") + L"\t … --pass 1 --bitrate\n"
		+ L"   " + String(x265_encodemode_desc[4]).ToString()->Replace(L"マルチパス - ", L"") + L"\t … --pass 3 --bitrate\n"
		+ L"\n"
		+ L"【" + String(x265_encodemode_desc[5]).ToString() + L"】\n"
		+ L"    マルチパス出力(1pass → npass)を自動で行います。\n"
		+ L"    --pass 1/3 --bitrate\n"
		+ L"\n"
		+ L"【" + String(x265_encodemode_desc[6]).ToString() + L"】\n"
		+ L"    品質基準VBR (crf)でのエンコード後、ファイルサイズ・ビットレートを確認します。\n"
		+ L"    --crf"
		);
	fcgTTX265->SetToolTip(fcgCBNulOut,            L"-o nul");
	fcgTTX265->SetToolTip(fcgCBFastFirstPass,     L"--slow-firstpass (チェックオフ時)");
	fcgTTX265->SetToolTip(fcgTXQuality,           L"--crf / --bitrate / --qp");
	fcgTTX265->SetToolTip(fcgTXStatusFile,        L"--stats");
	fcgTTX265->SetToolTip(fcgCXProfile,           L"--profile\n"
		+ L"\n"
		+ L"最終的にこの設定による制約が課されます。"
		);
	
	fcgTTX265->SetToolTip(fcgCBNulOut,            L"-o nul");
	fcgTTX265->SetToolTip(fcgCBFastFirstPass,     L"--slow-firstpass (チェックオフ時)");
	fcgTTX265->SetToolTip(fcgTXQuality,           L"--crf / --bitrate / --qp");
	fcgTTX265->SetToolTip(fcgTXStatusFile,        L"--stats");
	fcgTTX265->SetToolTip(fcgCXProfile,           L"--profile\n"
		+ L"\n"
		+ L"最終的にこの設定による制約が課されます。"
		);

	//自動マルチパス 上限設定
	String^ AMP_LimitBitrate = L""
		+ L"出力する動画ファイル(映像＋音声)のビットレートが、\n"
		+ L"ここで設定した上限ビットレートを超えないようエンコードを行います。\n"
		+ L"\n"
		+ L"エンコード終了後にも確認を行い、ファイルビットレートが上限設定を\n"
		+ L"上回ってしまった場合には、再エンコードを行います。\n"
		+ L"\n"
		+ L"上限設定はチェックボックスによりオン/オフできます。";
	fcgTTEx->SetToolTip(fcgCBAMPLimitBitrate,     AMP_LimitBitrate);
	fcgTTEx->SetToolTip(fcgNUAMPLimitBitrate,     AMP_LimitBitrate);
	String^ AMP_LimitFileSize = L""
		+ L"出力する動画ファイル(映像＋音声)のサイズが、\n"
		+ L"ここで設定した上限ファイルサイズを超えないようエンコードを行います。\n"
		+ L"\n"
		+ L"エンコード終了後にも確認を行い、ファイルサイズが上限設定を\n"
		+ L"上回ってしまった場合には、再エンコードを行います。\n"
		+ L"\n"
		+ L"上限設定はチェックボックスによりオン/オフできます。";
	fcgTTEx->SetToolTip(fcgCBAMPLimitFileSize,     AMP_LimitFileSize);
	fcgTTEx->SetToolTip(fcgNUAMPLimitFileSize,     AMP_LimitFileSize);
	
	//プロファイルとか
	fcgTTX265->SetToolTip(fcgCXTune,              L"--tune");
	fcgTTX265->SetToolTip(fcgCXPreset,            L"--preset");
	fcgTTEx->SetToolTip(fcgBTApplyPreset,         L""
		+ L"ボックス内で指定した\n"
		+ L"\n"
		+ L"・速度 (Preset)\n"
		+ L"・チューニング (tune)\n"
		+ L"・プロファイル (Profile)\n"
		+ L"\n"
		+ L"をGUIに適用します。"
		);
	
	fcgTTX265->SetToolTip(fcgCXAspectRatio,      L""
		+ String(aspect_desc[0]).ToString() + L"\n"
		+ L"   --sar を直接指定します。\n"
		+ L"\n"
		+ String(aspect_desc[1]).ToString() + L"\n"
		+ L"   エンコード時に 解像度から --sarを自動計算します。"
		);
	fcgTTX265->SetToolTip(fcgNUAspectRatioX,     L"アスペクト比 横 (幅)");
	fcgTTX265->SetToolTip(fcgNUAspectRatioY,     L"アスペクト比 縦 (高さ)");
	
	fcgTTX265->SetToolTip(fcgCXInterlaced,       L"--interlace");
	fcgTTX265->SetToolTip(fcgCXVideoFormat,      L"--videoformat");

	fcgTTX265->SetToolTip(fcgCXCSP,              L"--input-csp\n"
		+ L"通常は i420 を使用します。"
		);
	SetHelpToolTipsColorMatrixX265(fcgCXColorMatrix, "colormatrix");
	SetHelpToolTipsColorMatrixX265(fcgCXColorPrim,   "colorprim");
	SetHelpToolTipsColorMatrixX265(fcgCXTransfer,    "transfer");
	fcgTTX265->SetToolTip(fcgCBFullRange,        L"--range");
	
	fcgTTX265->SetToolTip(fcgNUVBVbuf,           L"--vbv-bufsize");
	fcgTTX265->SetToolTip(fcgNUVBVmax,           L"--vbv-maxrate");
	fcgTTX265->SetToolTip(fcgNUScenecut,         L"--scenecut");
	fcgTTX265->SetToolTip(fcgNUKeyintMin,        L"--min-keyint");
	fcgTTX265->SetToolTip(fcgNUKeyintMax,        L"--keyint");
	fcgTTX265->SetToolTip(fcgCBOpenGOP,          L"--open-gop");
	fcgTTX265->SetToolTip(fcgNURCLookahead,      L"--rc-lookahead");
	fcgTTX265->SetToolTip(fcgNURef,              L"--ref");
	fcgTTX265->SetToolTip(fcgNUBframes,          L"--bframes");
	fcgTTX265->SetToolTip(fcgCXBadapt,           L"--b-adapt");
	fcgTTX265->SetToolTip(fcgCBBpyramid,         L"--b-pyramid");
	fcgTTX265->SetToolTip(fcgCBWeightB,          L"--weightb");
	fcgTTX265->SetToolTip(fcgCBWeightP,          L"--weightp");

	fcgTTX265->SetToolTip(fcgNURD,               L"--rd");
	fcgTTX265->SetToolTip(fcgCXAQMode,           L"--aq-mode");
	fcgTTX265->SetToolTip(fcgNUAQStrength,       L"--aq-strength");
	fcgTTX265->SetToolTip(fcgNUPsyRD,            L"--psy-rd");
	fcgTTX265->SetToolTip(fcgCBCUTree,           L"--cutree");

	fcgTTX265->SetToolTip(fcgNUCtu,              L"--ctu");
	fcgTTX265->SetToolTip(fcgNUTuIntraDepth,     L"--tu-intra-depth");
	fcgTTX265->SetToolTip(fcgNUTuInterDepth,     L"--tu-inter-depth");
	fcgTTX265->SetToolTip(fcgCBWpp,              L"--wpp");

	fcgTTX265->SetToolTip(fcgCXME,               L"--me");
	fcgTTX265->SetToolTip(fcgCXSubME,            L"--subme");
	fcgTTX265->SetToolTip(fcgNUMERange,          L"--merange");
	fcgTTX265->SetToolTip(fcgNUMaxMerge,         L"--max-merge");
	fcgTTX265->SetToolTip(fcgCBAsymmetricMP,     L"--amp");
	fcgTTX265->SetToolTip(fcgCBRectMP,           L"--rect");

	fcgTTX265->SetToolTip(fcgNUThreads,          L"--threads\n"
		+ L"\"0\" で自動です。"
		);
	fcgTTX265->SetToolTip(fcgNUFrameThreads,     L"--frame-threads");

	fcgTTX265->SetToolTip(fcgCBLoopFilter,       L"--lft");
	fcgTTX265->SetToolTip(fcgCBSAO,              L"--sao");

	//拡張
	fcgTTEx->SetToolTip(fcgCBAFS,                L""
		+ L"自動フィールドシフト(afs)を使用してVFR化を行います。\n"
		+ L"エンコード時にタイムコードを作成し、mux時に埋め込んで\n"
		+ L"フレームレートを変更します。\n"
		+ L"\n"
		+ L"外部muxerの使用が必須となり、mp4出力時は\n"
		+ L"mp4boxとともにtc2mp4modが必要になりますので、\n"
		+ L"忘れずに指定してください。\n"
		+ L"\n"
		+ L"また、あとからフレームレートを変更するため、\n"
		+ L"x264のVBV設定が正確に反映されなくなる点に注意してください。"
		);
	fcgTTEx->SetToolTip(fcgCBAFSBitrateCorrection, L""
		+ L"自動フィールドシフト(afs)使用時にはmux時のフレームレート変更により\n"
		+ L"実レートが指定したビットレートからずれてしまいます。\n"
		+ L"\n"
		+ L"そこで、1pass目のdrop数から計算して\n"
		+ L"ビットレート指定値の補正を行います。\n"
		+ L"これにより最終的な実レートが設定通りの動画を作成できます。"
		);
	fcgTTEx->SetToolTip(fcgCBAuoTcfileout, L""
		+ L"タイムコードを出力します。このタイムコードは\n"
		+ L"自動フィールドシフト(afs)を反映したものになります。"
		);
	fcgTTEx->SetToolTip(fcgCBCheckKeyframes, L""
		+ L"Aviutlのキーフレーム設定をx264に伝えるため、\n"
		+ L"キーフレーム検出を行います。\n"
		+ L"\n"
		+ L"キーフレーム検出は、自動フィールドシフトと同時に使用できません。\n"
		+ L"自動フィールドシフト使用時は無効となります。"
		);
	fcgTTEx->SetToolTip(fcgCBSetKeyframeAtChapter, L""
		+ L"チャプターの位置にキーフレームを設定します。\n"
		+ L"\n"
		+ L"キーフレーム検出は、自動フィールドシフトと同時に使用できません。\n"
		+ L"自動フィールドシフト使用時は無効となります。"
		);
	fcgTTEx->SetToolTip(fcgCBInputAsLW48, L""
		+ L"L-SMASH WorksのLW48機能を使用します。\n"
		+ L"lwinput.auiおよびlwcolor.aucと組み合わせて使用します。\n"
		+ L"チェックを入れることで、通常のYC48ではなく、\n"
		+ L"LW48として処理するようになります。"
		);
	fcgTTEx->SetToolTip(fcgCXTempDir,      L""
		+ L"一時ファイル群\n"
		+ L"・音声一時ファイル(wav / エンコード後音声)\n"
		+ L"・動画一時ファイル\n"
		+ L"・タイムコードファイル\n"
		+ L"・qpファイル\n"
		+ L"・mux後ファイル\n"
		+ L"の作成場所を指定します。"
		);
	fcgTTEx->SetToolTip(fcgBTCustomTempDir, L""
		+ L"一時ファイルの場所を「カスタム」に設定した際に\n"
		+ L"使用される一時ファイルの場所を指定します。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);
	fcgTTEx->SetToolTip(fcgBTCmdEx,       L""
		+ L"入力したコマンド(オプション)をGUIに反映させます。"
		);

	//音声
	fcgTTEx->SetToolTip(fcgCXAudioEncoder, L""
		+ L"使用する音声エンコーダを指定します。\n"
		+ L"これらの設定はx265guiEx.iniに記述されています。"
		);
	fcgTTEx->SetToolTip(fcgCBAudioOnly,    L""
		+ L"動画の出力を行わず、音声エンコードのみ行います。\n"
		+ L"音声エンコードに失敗した場合などに使用してください。"
		);
	fcgTTEx->SetToolTip(fcgCBFAWCheck,     L""
		+ L"音声エンコード時に音声がFakeAACWav(FAW)かどうかの判定を行い、\n"
		+ L"FAWだと判定された場合、設定を無視して、\n"
		+ L"自動的にFAWを使用するよう切り替えます。\n"
		+ L"\n"
		+ L"一度音声エンコーダからFAW(fawcl)を選択し、\n"
		+ L"実行ファイルの場所を指定しておく必要があります。"
		);
	fcgTTEx->SetToolTip(fcgBTAudioEncoderPath, L""
		+ L"音声エンコーダの場所を指定します。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);
	fcgTTEx->SetToolTip(fcgCXAudioEncMode, L""
		+ L"音声エンコーダのエンコードモードを切り替えます。\n"
		+ L"これらの設定はx265guiEx.iniに記述されています。"
		);
	fcgTTEx->SetToolTip(fcgCBAudio2pass,   L""
		+ L"音声エンコードを2passで行います。\n"
		+ L"2pass時はパイプ処理は行えません。"
		);
	fcgTTEx->SetToolTip(fcgCBAudioUsePipe, L""
		+ L"パイプを通して、音声データをエンコーダに渡します。\n"
		+ L"パイプと2passは同時に指定できません。"
		);
	fcgTTEx->SetToolTip(fcgNUAudioBitrate, L""
		+ L"音声ビットレートを指定します。"
		);
	fcgTTEx->SetToolTip(fcgCXAudioPriority, L""
		+ L"音声エンコーダのCPU優先度を設定します。\n"
		+ L"AviutlSync で Aviutlの優先度と同じになります。"
		);
	fcgTTEx->SetToolTip(fcgCXAudioEncTiming, L""
		+ L"音声を処理するタイミングを設定します。\n"
		+ L" 後　 … 映像→音声の順で処理します。\n"
		+ L" 前　 … 音声→映像の順で処理します。\n"
		+ L" 同時 … 映像と音声を同時に処理します。"
		);
	fcgTTEx->SetToolTip(fcgCXAudioTempDir, L""
		+ L"音声一時ファイル(エンコード後のファイル)\n"
		+ L"の出力先を変更します。"
		);
	fcgTTEx->SetToolTip(fcgBTCustomAudioTempDir, L""
		+ L"音声一時ファイルの場所を「カスタム」にした時に\n"
		+ L"使用される音声一時ファイルの場所を指定します。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);

	//muxer
	fcgTTEx->SetToolTip(fcgCBMP4MuxerExt, L""
		+ L"指定したmuxerでmuxを行います。\n"
		+ L"チェックを外すとmuxを行いません。"
		);
	fcgTTEx->SetToolTip(fcgCXMP4CmdEx,    L""
		+ L"muxerに渡す追加オプションを選択します。\n"
		+ L"これらの設定はx265guiEx.iniに記述されています。"
		);
	fcgTTEx->SetToolTip(fcgBTMP4MuxerPath, L""
		+ L"mp4用muxerの場所を指定します。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);
	fcgTTEx->SetToolTip(fcgBTTC2MP4Path, L""
		+ L"tc2mp4modの場所を指定します。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);
	fcgTTEx->SetToolTip(fcgBTMP4RawPath, L""
		+ L"raw用mp4muxerの場所を指定します。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);
	fcgTTEx->SetToolTip(fcgCXMP4BoxTempDir, L""
		+ L"mp4box用の一時フォルダの場所を指定します。"
		);
	fcgTTEx->SetToolTip(fcgBTMP4BoxTempDir, L""
		+ L"mp4box用一時フォルダの場所を「カスタム」に設定した際に\n"
		+ L"使用される一時フォルダの場所です。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);
	fcgTTEx->SetToolTip(fcgCBMKVMuxerExt, L""
		+ L"指定したmuxerでmuxを行います。\n"
		+ L"チェックを外すとmuxを行いません。"
		);
	fcgTTEx->SetToolTip(fcgCXMKVCmdEx,    L""
		+ L"muxerに渡す追加オプションを選択します。\n"
		+ L"これらの設定はx265guiEx.iniに記述されています。"
		);
	fcgTTEx->SetToolTip(fcgBTMKVMuxerPath, L""
		+ L"mkv用muxerの場所を指定します。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);
	fcgTTEx->SetToolTip(fcgCBMPGMuxerExt, L""
		+ L"指定したmuxerでmuxを行います。\n"
		+ L"チェックを外すとmuxを行いません。"
		);
	fcgTTEx->SetToolTip(fcgCXMPGCmdEx,    L""
		+ L"muxerに渡す追加オプションを選択します。\n"
		+ L"これらの設定はx265guiEx.iniに記述されています。"
		);
	fcgTTEx->SetToolTip(fcgBTMPGMuxerPath, L""
		+ L"mpg用muxerの場所を指定します。\n"
		+ L"\n"
		+ L"この設定はx265guiEx.confに保存され、\n"
		+ L"バッチ処理ごとの変更はできません。"
		);
	fcgTTEx->SetToolTip(fcgCXMuxPriority, L""
		+ L"muxerのCPU優先度を指定します。\n"
		+ L"AviutlSync で Aviutlの優先度と同じになります。"
		);
	//バッチファイル実行
	fcgTTEx->SetToolTip(fcgCBRunBatBefore, L""
		+ L"エンコード開始前にバッチファイルを実行します。"
		);
	fcgTTEx->SetToolTip(fcgCBRunBatAfter, L""
		+ L"エンコード終了後、バッチファイルを実行します。"
		);
	fcgTTEx->SetToolTip(fcgCBWaitForBatBefore, L""
		+ L"バッチ処理開始後、バッチ処理が終了するまで待機します。"
		);
	fcgTTEx->SetToolTip(fcgCBWaitForBatAfter, L""
		+ L"バッチ処理開始後、バッチ処理が終了するまで待機します。"
		);
	fcgTTEx->SetToolTip(fcgBTBatBeforePath, L""
		+ L"エンコード終了後実行するバッチファイルを指定します。\n"
		+ L"実際のバッチ実行時には新たに\"<バッチファイル名>_tmp.bat\"を作成、\n"
		+ L"指定したバッチファイルの内容をコピーし、\n"
		+ L"さらに特定文字列を置換して実行します。\n"
		+ L"使用できる置換文字列はreadmeをご覧下さい。"
		);
	fcgTTEx->SetToolTip(fcgBTBatAfterPath, L""
		+ L"エンコード終了後実行するバッチファイルを指定します。\n"
		+ L"実際のバッチ実行時には新たに\"<バッチファイル名>_tmp.bat\"を作成、\n"
		+ L"指定したバッチファイルの内容をコピーし、\n"
		+ L"さらに特定文字列を置換して実行します。\n"
		+ L"使用できる置換文字列はreadmeをご覧下さい。"
		);
	//上部ツールストリップ
	fcgTSBCMDOnly->ToolTipText = L""
		+ L"GUIモード と CLIモードを切り替えます。\n"
		+ L"CLIモードはコマンドライン入力のみで\n"
		+ L"オプションの指定を行います。";

	fcgTSBDelete->ToolTipText = L""
		+ L"現在選択中のプロファイルを削除します。";

	fcgTSBOtherSettings->ToolTipText = L""
		+ L"プロファイルの保存フォルダを変更します。";

	fcgTSBSave->ToolTipText = L""
		+ L"現在の設定をプロファイルに上書き保存します。";

	fcgTSBSaveNew->ToolTipText = L""
		+ L"現在の設定を新たなプロファイルに保存します。";

	//他
	fcgTTEx->SetToolTip(fcgTXCmd,         L""
		+ L"x264に渡される予定のコマンドラインです。\n"
		+ L"エンコード時には更に\n"
		+ L"・「追加コマンド」の付加\n"
		+ L"・\"auto\"な設定項目の反映\n"
		+ L"・必要な情報の付加(--fps/-o/--input-res/--input-csp/--frames等)\n"
		+ L"が行われます。\n"
		+ L"\n"
		+ L"このウィンドウはダブルクリックで拡大縮小できます。"
		);
	fcgTTEx->SetToolTip(fcgBTDefault,     L""
		+ L"デフォルト設定をロードします。"
		);
}
System::Void frmConfig::SetX265VersionToolTip(String^ x265Path, bool ashighbit) {
	String^ mes;
	if (File::Exists(x265Path)) {
		char mes_buf[2560];
		char exe_path[MAX_PATH_LEN];
		GetCHARfromString(exe_path, sizeof(exe_path), x265Path);
		if (get_exe_message(exe_path, "-V", mes_buf, _countof(mes_buf), AUO_PIPE_ENABLE) == RP_SUCCESS)
			mes = String(mes_buf).ToString();
		else
			mes = L"バージョン情報の取得に失敗しました。";
	} else {
		mes = L"指定されたx265が存在しません。";
	}
	if (ashighbit == fcgCBUsehighbit->Checked)
		fcgTTX265Version->SetToolTip(fcgTXX265Path, mes);

	fcgTTX265Version->SetToolTip((ashighbit) ? fcgTXX265PathSubhighbit : fcgTXX265PathSub, mes);
}
System::Void frmConfig::ShowExehelp(String^ ExePath, String^ args) {
	if (!File::Exists(ExePath)) {
		MessageBox::Show(L"指定された実行ファイルが存在しません。", L"エラー", MessageBoxButtons::OK, MessageBoxIcon::Error);
	} else {
		char exe_path[MAX_PATH_LEN];
		char file_path[MAX_PATH_LEN];
		char cmd[MAX_CMD_LEN];
		GetCHARfromString(exe_path, sizeof(exe_path), ExePath);
		apply_appendix(file_path, _countof(file_path), exe_path, "_fullhelp.txt");
		File::Delete(String(file_path).ToString());
		array<String^>^ arg_list = args->Split(L';');
		for (int i = 0; i < arg_list->Length; i++) {
			if (i) {
				StreamWriter^ sw;
				try {
					sw = gcnew StreamWriter(String(file_path).ToString(), true, System::Text::Encoding::GetEncoding("shift_jis"));
					sw->WriteLine();
					sw->WriteLine();
				} catch (...) {
					//ファイルオープンに失敗…初回のget_exe_message_to_fileでエラーとなるため、おそらく起こらない
				} finally {
					if (sw != nullptr) { sw->Close(); }
				}
			}
			GetCHARfromString(cmd, sizeof(cmd), arg_list[i]);
			if (get_exe_message_to_file(exe_path, cmd, file_path, AUO_PIPE_MUXED, 5) != RP_SUCCESS) {
				File::Delete(String(file_path).ToString());
				MessageBox::Show(L"helpの取得に失敗しました。", L"エラー", MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			}
		}
		try {
			System::Diagnostics::Process::Start(String(file_path).ToString());
		} catch (...) {
			MessageBox::Show(L"helpを開く際に不明なエラーが発生しました。", L"エラー", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
	}
}

#pragma warning( pop )
