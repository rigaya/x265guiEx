// -----------------------------------------------------------------------------------------
// x264guiEx/x265guiEx/svtAV1guiEx/ffmpegOut/QSVEnc/NVEnc/VCEEnc by rigaya
// -----------------------------------------------------------------------------------------
// The MIT License
//
// Copyright (c) 2010-2022 rigaya
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// --------------------------------------------------------------------------------------------

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
        MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_INVALID_CHAR), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
        return false;
    }
    if (String::Compare(Path::GetExtension(stgName), L".stg", true))
        stgName += L".stg";
    if (File::Exists(fileName = Path::Combine(fsnCXFolderBrowser->GetSelectedFolder(), stgName)))
        if (MessageBox::Show(stgName + LOAD_CLI_STRING(AUO_CONFIG_ALREADY_EXISTS), LOAD_CLI_STRING(AUO_CONFIG_OVERWRITE_CHECK), MessageBoxButtons::YesNo, MessageBoxIcon::Question)
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
System::Void frmBitrateCalculator::Init(int VideoBitrate, int AudioBitrate, bool BTVBEnable, bool BTABEnable, int ab_max, const AuoTheme themeTo, const DarkenWindowStgReader *dwStg) {
    guiEx_settings exStg(true);
    exStg.load_fbc();
    enable_events = false;
    dwStgReader = dwStg;
    CheckTheme(themeTo);
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
System::Void frmBitrateCalculator::CheckTheme(const AuoTheme themeTo) {
    //変更の必要がなければ終了
    if (themeTo == themeMode) return;

    //一度ウィンドウの再描画を完全に抑止する
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 0, 0);
    SetAllColor(this, themeTo, this->GetType(), dwStgReader);
    SetAllMouseMove(this, themeTo);
    //一度ウィンドウの再描画を再開し、強制的に再描画させる
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 1, 0);
    this->Refresh();
    themeMode = themeTo;
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
System::Void frmBitrateCalculator::fbcMouseEnter_SetColor(System::Object^  sender, System::EventArgs^  e) {
    fcgMouseEnterLeave_SetColor(sender, themeMode, DarkenWindowState::Hot, dwStgReader);
}
System::Void frmBitrateCalculator::fbcMouseLeave_SetColor(System::Object^  sender, System::EventArgs^  e) {
    fcgMouseEnterLeave_SetColor(sender, themeMode, DarkenWindowState::Normal, dwStgReader);
}
System::Void frmBitrateCalculator::SetAllMouseMove(Control ^top, const AuoTheme themeTo) {
    if (themeTo == themeMode) return;
    System::Type^ type = top->GetType();
    if (type == CheckBox::typeid) {
        top->MouseEnter += gcnew System::EventHandler(this, &frmBitrateCalculator::fbcMouseEnter_SetColor);
        top->MouseLeave += gcnew System::EventHandler(this, &frmBitrateCalculator::fbcMouseLeave_SetColor);
    }
    for (int i = 0; i < top->Controls->Count; i++) {
        SetAllMouseMove(top->Controls[i], themeTo);
    }
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
            (int)fcgNUAudioBitrate->Maximum,
            themeMode,
            dwStgReader
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
///     frmConfig 関数
/// -------------------------------------------------
/////////////   LocalStg関連  //////////////////////
System::Void frmConfig::LoadLocalStg() {
    guiEx_settings *_ex_stg = sys_dat->exstg;
    _ex_stg->load_encode_stg();
    LocalStg.x265ExeName     = String(_ex_stg->s_enc.filename).ToString();
    LocalStg.x265Path        = String(_ex_stg->s_enc.fullpath).ToString();
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
    if (fcgTSBCMDOnly->Checked) {
        //CLIモードの時はコマンドラインを解析してhighbitかどうか判定
        CONF_GUIEX cnf;
        init_CONF_GUIEX(&cnf, FALSE);
        char cmdex[2048] = { 0 };
        GetCHARfromString(cmdex, sizeof(cmdex), fcgTXCmdEx->Text);
        set_cmd_to_conf(cmdex, &cnf.enc);
    }
    if (LocalStg.x265Path->Length > 0
        && !File::Exists(LocalStg.x265Path)) {
        error = true;
        err += LOAD_CLI_STRING(AUO_CONFIG_VID_ENC_NOT_EXIST) + L"\n [ " + LocalStg.x265Path + L" ]\n";
    }
    //音声エンコーダのチェック (実行ファイル名がない場合はチェックしない)
    if (LocalStg.audEncExeName[fcgCXAudioEncoder->SelectedIndex]->Length) {
        String^ AudioEncoderPath = LocalStg.audEncPath[fcgCXAudioEncoder->SelectedIndex];
        if (AudioEncoderPath->Length > 0
            && !File::Exists(AudioEncoderPath)
            && (fcgCXAudioEncoder->SelectedIndex != sys_dat->exstg->s_aud_faw_index)) {
            //音声実行ファイルがない かつ
            //選択された音声がfawでない または fawであってもfaw2aacがない
            if (error) err += L"\n\n";
            error = true;
            err += LOAD_CLI_STRING(AUO_CONFIG_AUD_ENC_NOT_EXIST) + L"\n [ " + AudioEncoderPath + L" ]\n";
        }
    }
    //FAWのチェック
    if (fcgCBFAWCheck->Checked) {
        if (sys_dat->exstg->s_aud_faw_index == FAW_INDEX_ERROR) {
            if (error) err += L"\n\n";
            error = true;
            err += LOAD_CLI_STRING(AUO_CONFIG_FAW_STG_NOT_FOUND_IN_INI1) + L"\n"
                +  LOAD_CLI_STRING(AUO_CONFIG_FAW_STG_NOT_FOUND_IN_INI2) + L"\n"
                +  LOAD_CLI_STRING(AUO_CONFIG_FAW_STG_NOT_FOUND_IN_INI3);
        }
    }
    //自動マルチパスの自動ビットレート設定のチェック
    if (fcgLBAMPAutoBitrate != nullptr && fcgLBAMPAutoBitrate->Visible) {
        if (error) err += L"\n\n";
        error = true;
        err += LOAD_CLI_STRING(AUO_CONFIG_AMP_STG_INVALID1) + L"\n"
            +  LOAD_CLI_STRING(AUO_CONFIG_AMP_STG_INVALID2) + L"\n"
            +  LOAD_CLI_STRING(AUO_CONFIG_AMP_STG_INVALID3) + L"\n"
            +  LOAD_CLI_STRING(AUO_CONFIG_AMP_STG_INVALID4);
    }
    if (error)
        MessageBox::Show(this, err, LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
    return error;
}

System::Void frmConfig::SaveLocalStg() {
    guiEx_settings *_ex_stg = sys_dat->exstg;
    _ex_stg->load_encode_stg();
    _ex_stg->s_local.large_cmdbox = fcgTXCmd->Multiline;
    GetCHARfromString(_ex_stg->s_enc.fullpath,               sizeof(_ex_stg->s_enc.fullpath),               LocalStg.x265Path);
    GetCHARfromString(_ex_stg->s_local.custom_tmp_dir,        sizeof(_ex_stg->s_local.custom_tmp_dir),        LocalStg.CustomTmpDir);
    GetCHARfromString(_ex_stg->s_local.custom_mp4box_tmp_dir, sizeof(_ex_stg->s_local.custom_mp4box_tmp_dir), LocalStg.CustomMP4TmpDir);
    GetCHARfromString(_ex_stg->s_local.custom_audio_tmp_dir,  sizeof(_ex_stg->s_local.custom_audio_tmp_dir),  LocalStg.CustomAudTmpDir);
    GetCHARfromString(_ex_stg->s_local.app_dir,               sizeof(_ex_stg->s_local.app_dir),               LocalStg.LastAppDir);
    GetCHARfromString(_ex_stg->s_local.bat_dir,               sizeof(_ex_stg->s_local.bat_dir),               LocalStg.LastBatDir);
    GetCHARfromString(_ex_stg->s_mux[MUXER_MP4].fullpath,     sizeof(_ex_stg->s_mux[MUXER_MP4].fullpath),     LocalStg.MP4MuxerPath);
    GetCHARfromString(_ex_stg->s_mux[MUXER_MKV].fullpath,     sizeof(_ex_stg->s_mux[MUXER_MKV].fullpath),     LocalStg.MKVMuxerPath);
    GetCHARfromString(_ex_stg->s_mux[MUXER_TC2MP4].fullpath,  sizeof(_ex_stg->s_mux[MUXER_TC2MP4].fullpath),  LocalStg.TC2MP4Path);
    GetCHARfromString(_ex_stg->s_mux[MUXER_MP4_RAW].fullpath, sizeof(_ex_stg->s_mux[MUXER_MP4_RAW].fullpath), LocalStg.MP4RawPath);
    for (int i = 0; i < _ex_stg->s_aud_count; i++)
        GetCHARfromString(_ex_stg->s_aud[i].fullpath,         sizeof(_ex_stg->s_aud[i].fullpath),             LocalStg.audEncPath[i]);
    _ex_stg->save_local();
}

System::Void frmConfig::SetLocalStg() {
    fcgLBX265Path->Text           = System::String(ENCODER_NAME).ToString() + L".exe" + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
    fcgLBX265PathSub->Text        = System::String(ENCODER_NAME).ToString() + L".exe" + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
    fcgTXX265Path->Text           = LocalStg.x265Path;
    fcgTXX265PathSub->Text        = LocalStg.x265Path;

    fcgTXMP4MuxerPath->Text       = LocalStg.MP4MuxerPath;
    fcgTXMKVMuxerPath->Text       = LocalStg.MKVMuxerPath;
    fcgTXTC2MP4Path->Text         = LocalStg.TC2MP4Path;
    fcgTXMP4RawPath->Text         = LocalStg.MP4RawPath;
    fcgTXCustomAudioTempDir->Text = LocalStg.CustomAudTmpDir;
    fcgTXCustomTempDir->Text      = LocalStg.CustomTmpDir;
    fcgTXMP4BoxTempDir->Text      = LocalStg.CustomMP4TmpDir;
    fcgLBMP4MuxerPath->Text       = LocalStg.MP4MuxerExeName + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
    fcgLBMKVMuxerPath->Text       = LocalStg.MKVMuxerExeName + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
    fcgLBTC2MP4Path->Text         = LocalStg.TC2MP4ExeName   + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
    fcgLBMP4RawPath->Text         = LocalStg.MP4RawExeName   + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);

    fcgTXX265Path->SelectionStart           = fcgTXX265Path->Text->Length;
    fcgTXX265PathSub->SelectionStart        = fcgTXX265PathSub->Text->Length;
    fcgTXMP4MuxerPath->SelectionStart       = fcgTXMP4MuxerPath->Text->Length;
    fcgTXTC2MP4Path->SelectionStart         = fcgTXTC2MP4Path->Text->Length;
    fcgTXMKVMuxerPath->SelectionStart       = fcgTXMKVMuxerPath->Text->Length;
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
    frmOtherSettings::Instance::get()->SetTheme(themeMode, dwStgReader);
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
    LoadLangText();
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
        fcgtabControlVideo->TabPages->Insert(2, fcgtabPageX2652pass);
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
    //一度ウィンドウの再描画を完全に抑止する
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 0, 0);
    fcgCXX265Mode_SelectedIndexChanged(sender, e);
    fcgTXX265Path->Text = LocalStg.x265Path;
    fcgTXX265Path->SelectionStart = fcgTXX265Path->Text->Length;
    fcgLBX265Path->Text = L"x265.exe の指定";
    SetX265VersionToolTip(fcgTXX265Path->Text);
    //一度ウィンドウの再描画を再開し、強制的に再描画させる
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 1, 0);
    this->Refresh();
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
            fcgLBQuality->Text = (fcgCXX265Mode->SelectedIndex == 5) ? LOAD_CLI_STRING(AUO_CONFIG_MODE_TARGET_BITRATE) : LOAD_CLI_STRING(AUO_CONFIG_MODE_BITRATE);
            fcgLBQualityLeft->Text = LOAD_CLI_STRING(AUO_CONFIG_QUALITY_LOW);
            fcgLBQualityRight->Text = LOAD_CLI_STRING(AUO_CONFIG_QUALITY_HIGH);
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
            fcgLBQuality->Text = LOAD_CLI_STRING(AUO_CONFIG_MODE_QP);
            fcgLBQualityLeft->Text = LOAD_CLI_STRING(AUO_CONFIG_QUALITY_HIGH);
            fcgLBQualityRight->Text = LOAD_CLI_STRING(AUO_CONFIG_QUALITY_LOW);
            fcgTBQuality->Minimum = -1;
            fcgTBQuality->Maximum = 51;
            fcgCBNulOut->Enabled = false; //Enabledの変更が先
            fcgCBNulOut->Checked = false;
            fcgCBFastFirstPass->Enabled = false; //Enabledの変更が先
            fcgCBFastFirstPass->Checked = false;
            fcgTXQuality->Text = Convert::ToString(cnf_fcgTemp->qp);
            SetfbcBTVBEnable(false);
            break;
        case X265_RC_CRF:
        default:
            fcgLBQuality->Text = LOAD_CLI_STRING(AUO_CONFIG_MODE_CRF);
            fcgLBQualityLeft->Text = LOAD_CLI_STRING(AUO_CONFIG_QUALITY_HIGH);
            fcgLBQualityRight->Text =  LOAD_CLI_STRING(AUO_CONFIG_QUALITY_LOW);
            fcgTBQuality->Minimum = (get_bit_depth(fcgCXBitDepth->SelectedIndex) > 8) ? -12*2 : 0;
            fcgTBQuality->Maximum = 51*2;
            fcgCBNulOut->Enabled = false; //Enabledの変更が先
            fcgCBNulOut->Checked = false;
            fcgCBFastFirstPass->Enabled = false; //Enabledの変更が先
            fcgCBFastFirstPass->Checked = false;
            fcgTXQuality->Text = Convert::ToString(clamp(cnf_fcgTemp->crf / 100.0, (get_bit_depth(fcgCXBitDepth->SelectedIndex) > 8) ? -12 : 0, 51));
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
        MessageBox::Show(this, LOAD_CLI_STRING(AUO_CONFIG_TEXT_LIMIT_LENGTH), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
        fcgTSTSettingsNotes->Focus();
        fcgTSTSettingsNotes->SelectionStart = fcgTSTSettingsNotes->Text->Length;
        return false;
    }
    fcgTSTSettingsNotes->Visible = Enable;
    fcgTSLSettingsNotes->Visible = !Enable;
    if (Enable) {
        fcgTSTSettingsNotes->Text = fcgTSLSettingsNotes->Text;
        fcgTSTSettingsNotes->Focus();
        bool isDefaultNote = fcgTSLSettingsNotes->Overflow != ToolStripItemOverflow::Never;
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
        bool ret = (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK);
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
        maxwidth = std::max(maxwidth, ds->MeasureString(CX->Items[i]->ToString(), CX->Font).Width);
    CX->DropDownWidth = (int)(maxwidth + 0.5);
    delete ds;
}

System::Void frmConfig::InitCXCmdExInsert() {
    fcgCXCmdExInsert->BeginUpdate();
    fcgCXCmdExInsert->Items->Clear();
    fcgCXCmdExInsert->Items->Add(LOAD_CLI_STRING(AUO_CONFIG_INSERT_STRING));
    fcgCXCmdExInsert->Items->Add(LOAD_CLI_STRING(AUO_CONFIG_FILE_FULL_PATH));
    System::Drawing::Graphics^ ds = fcgCXCmdExInsert->CreateGraphics();
    float max_width_of_string = 0;
    for (int i = 0; REPLACE_STRINGS_LIST[i].desc; i++)
        max_width_of_string = std::max(max_width_of_string, ds->MeasureString(String(REPLACE_STRINGS_LIST[i].string).ToString() + L" … ", fcgCXCmdExInsert->Font).Width);
    for (int i = 0; REPLACE_STRINGS_LIST[i].desc; i++) {
        String^ AppenStr = String(REPLACE_STRINGS_LIST[i].string).ToString();
        const int length_of_string = AppenStr->Length;
        AppenStr += L" … ";
        for (float current_width = 0.0; current_width < max_width_of_string; AppenStr = AppenStr->Insert(length_of_string, L" "))
            current_width = ds->MeasureString(AppenStr, fcgCXCmdExInsert->Font).Width;
        String^ descStr = LOAD_CLI_STRING(REPLACE_STRINGS_LIST[i].mes);
        if (descStr->Length == 0) {
            descStr = String(REPLACE_STRINGS_LIST[i].desc).ToString();
        }
        AppenStr += descStr;
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
        fcgLBAudioEncoderPath->Text = String(astg->filename).ToString() + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
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
        for (int i = 0; i < items_to_set; i++) {
            String^ string = nullptr;
            if (AUDIO_DELAY_CUT_MODE[i].mes != AUO_MES_UNKNOWN) {
                string = LOAD_CLI_STRING(AUDIO_DELAY_CUT_MODE[i].mes);
            }
            if (string == nullptr || string->Length == 0) {
                string = String(AUDIO_DELAY_CUT_MODE[i].desc).ToString();
            }
            fcgCXAudioDelayCut->Items->Add(string);
        }
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
    fcgTSSettings->Text = (mItem == nullptr) ? LOAD_CLI_STRING(AUO_CONFIG_PROFILE) : mItem->Text;
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
    init_CONF_GUIEX(cnf_stgSelected, get_bit_depth(fcgCXBitDepth->SelectedIndex) > 8);
    FrmToConf(cnf_stgSelected);
    String^ stgDir = Path::GetDirectoryName(stgName);
    if (!Directory::Exists(stgDir))
        Directory::CreateDirectory(stgDir);
    int result = guiEx_config::save_guiEx_conf(cnf_stgSelected, stg_name);
    free(stg_name);
    switch (result) {
        case CONF_ERROR_FILE_OPEN:
            MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_OPEN_STG_FILE), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        case CONF_ERROR_INVALID_FILENAME:
            MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_INVALID_CHAR), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        default:
            break;
    }
    init_CONF_GUIEX(cnf_stgSelected, get_bit_depth(fcgCXBitDepth->SelectedIndex) > 8);
    FrmToConf(cnf_stgSelected);
}

System::Void frmConfig::fcgTSBSave_Click(System::Object^  sender, System::EventArgs^  e) {
    if (CheckedStgMenuItem != nullptr)
        SaveToStgFile(CheckedStgMenuItem->Tag->ToString());
    CheckTSSettingsDropDownItem(CheckedStgMenuItem);
}

System::Void frmConfig::fcgTSBSaveNew_Click(System::Object^  sender, System::EventArgs^  e) {
    frmSaveNewStg::Instance::get()->setStgDir(String(sys_dat->exstg->s_local.stg_dir).ToString());
    frmSaveNewStg::Instance::get()->SetTheme(themeMode, dwStgReader);
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
        MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ASK_STG_FILE_DELETE) + L"[" + mItem->Text + L"]",
        LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OKCancel, MessageBoxIcon::Exclamation))
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
    char stg_path[MAX_PATH_LEN];
    GetCHARfromString(stg_path, sizeof(stg_path), ClickedMenuItem->Tag->ToString());
    if (guiEx_config::load_guiEx_conf(&load_stg, stg_path) == CONF_ERROR_FILE_OPEN) {
        if (MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_OPEN_STG_FILE) + L"\n"
                           + LOAD_CLI_STRING(AUO_CONFIG_ASK_STG_FILE_DELETE),
                           LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::YesNo, MessageBoxIcon::Error)
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

///////////////   言語ファイル関連   //////////////////////

System::Void frmConfig::CheckTSLanguageDropDownItem(ToolStripMenuItem^ mItem) {
    UncheckAllDropDownItem(fcgTSLanguage);
    fcgTSLanguage->Text = (mItem == nullptr) ? LOAD_CLI_STRING(AuofcgTSSettings) : mItem->Text;
    if (mItem != nullptr)
        mItem->Checked = true;
}
System::Void frmConfig::SetSelectedLanguage(const char *language_text) {
    for (int i = 0; i < fcgTSLanguage->DropDownItems->Count; i++) {
        ToolStripMenuItem^ item = dynamic_cast<ToolStripMenuItem^>(fcgTSLanguage->DropDownItems[i]);
        char item_text[MAX_PATH_LEN];
        GetCHARfromString(item_text, sizeof(item_text), item->Tag->ToString());
        if (strncmp(item_text, language_text, strlen(language_text)) == 0) {
            CheckTSLanguageDropDownItem(item);
            break;
        }
    }
}

System::Void frmConfig::SaveSelectedLanguage(const char *language_text) {
    sys_dat->exstg->set_and_save_lang(language_text);
}

System::Void frmConfig::fcgTSLanguage_DropDownItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
    ToolStripMenuItem^ ClickedMenuItem = dynamic_cast<ToolStripMenuItem^>(e->ClickedItem);
    if (ClickedMenuItem == nullptr)
        return;
    if (ClickedMenuItem->Tag == nullptr || ClickedMenuItem->Tag->ToString()->Length == 0)
        return;

    char language_text[MAX_PATH_LEN];
    GetCHARfromString(language_text, sizeof(language_text), ClickedMenuItem->Tag->ToString());
    SaveSelectedLanguage(language_text);
    load_lng(language_text);
    overwrite_aviutl_ini_auo_info();
    LoadLangText();
    CheckTSLanguageDropDownItem(ClickedMenuItem);
}

System::Void frmConfig::InitLangList() {
    if (list_lng != nullptr) {
        delete list_lng;
    }
#define ENABLE_LNG_FILE_DETECT 1
#if ENABLE_LNG_FILE_DETECT
    auto lnglist = find_lng_files();
    list_lng = new std::vector<std::string>();
    for (const auto& lang : lnglist) {
        list_lng->push_back(lang);
    }
#endif

    fcgTSLanguage->DropDownItems->Clear();

    for (const auto& auo_lang : list_auo_languages) {
        String^ label = String(auo_lang.code).ToString() + L" (" + String(auo_lang.name).ToString() + L")";
        ToolStripMenuItem^ mItem = gcnew ToolStripMenuItem(label);
        mItem->DropDownItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &frmConfig::fcgTSLanguage_DropDownItemClicked);
        mItem->Tag = String(auo_lang.code).ToString();
        fcgTSLanguage->DropDownItems->Add(mItem);
    }
#if ENABLE_LNG_FILE_DETECT
    for (size_t i = 0; i < list_lng->size(); i++) {
        auto filename = String(PathFindFileNameA((*list_lng)[i].c_str())).ToString();
        ToolStripMenuItem^ mItem = gcnew ToolStripMenuItem(filename);
        mItem->DropDownItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &frmConfig::fcgTSLanguage_DropDownItemClicked);
        mItem->Tag = filename;
        fcgTSLanguage->DropDownItems->Add(mItem);
    }
#endif
    SetSelectedLanguage(sys_dat->exstg->get_lang());
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
    setComboBox(fcgCXBitDepth,       bit_depth_desc);
    setComboBox(fcgCXCSP,            list_output_csp_x265);
    setComboBox(fcgCXAQMode,         list_aq);
    setComboBox(fcgCXAspectRatio,    aspect_desc);
    setComboBox(fcgCXX265Mode,       x265_encodemode_desc);
    setComboBox(fcgCXSceneCutMode,   x265_scenecut_mode_desc);
    setComboBox(fcgCXME,             list_me_x265);
    setComboBox(fcgCXSubME,          list_subme_x265);
    setComboBox(fcgCXBadapt,         list_b_adpat);
    setComboBox(fcgCXPreset,         sys_dat->exstg->s_enc.preset.name);
    setComboBox(fcgCXProfile,        sys_dat->exstg->s_enc.profile.name);
    setComboBox(fcgCXTune,             sys_dat->exstg->s_enc.tune.name);
    setComboBox(fcgCXInterlaced,     interlaced_desc);
    setComboBox(fcgCXTransfer,       list_transfer);
    setComboBox(fcgCXColorMatrix,    list_colormatrix);
    setComboBox(fcgCXColorPrim,      list_colorprim);
    setComboBox(fcgCXVideoFormat,    list_videoformat);
    setComboBox(fcgCXVideoFormat,    list_videoformat);

    setComboBox(fcgCXAudioEncTiming, audio_enc_timing_desc);
    setComboBox(fcgCXAudioDelayCut,  AUDIO_DELAY_CUT_MODE);

    InitCXCmdExInsert();

    setMuxerCmdExNames(fcgCXMP4CmdEx, MUXER_MP4);
    setMuxerCmdExNames(fcgCXMKVCmdEx, MUXER_MKV);

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
    SetTXMaxLen(fcgTXX265Path,             sizeof(sys_dat->exstg->s_enc.fullpath) - 1);
    SetTXMaxLen(fcgTXX265PathSub,          sizeof(sys_dat->exstg->s_enc.fullpath) - 1);
    SetTXMaxLen(fcgTXAudioEncoderPath,     sizeof(sys_dat->exstg->s_aud[0].fullpath) - 1);
    SetTXMaxLen(fcgTXMP4MuxerPath,         sizeof(sys_dat->exstg->s_mux[MUXER_MP4].fullpath) - 1);
    SetTXMaxLen(fcgTXMKVMuxerPath,         sizeof(sys_dat->exstg->s_mux[MUXER_MKV].fullpath) - 1);
    SetTXMaxLen(fcgTXTC2MP4Path,           sizeof(sys_dat->exstg->s_mux[MUXER_TC2MP4].fullpath) - 1);
    SetTXMaxLen(fcgTXMP4RawPath,           sizeof(sys_dat->exstg->s_mux[MUXER_MP4_RAW].fullpath) - 1);
    SetTXMaxLen(fcgTXCustomTempDir,        sizeof(sys_dat->exstg->s_local.custom_tmp_dir) - 1);
    SetTXMaxLen(fcgTXCustomAudioTempDir,   sizeof(sys_dat->exstg->s_local.custom_audio_tmp_dir) - 1);
    SetTXMaxLen(fcgTXMP4BoxTempDir,        sizeof(sys_dat->exstg->s_local.custom_mp4box_tmp_dir) - 1);
    SetTXMaxLen(fcgTXStatusFile,           sizeof(conf->vid.stats) - 1);
    SetTXMaxLen(fcgTXBatBeforeAudioPath,   sizeof(conf->oth.batfile.before_audio) - 1);
    SetTXMaxLen(fcgTXBatAfterAudioPath,    sizeof(conf->oth.batfile.after_audio) - 1);
    SetTXMaxLen(fcgTXBatBeforePath,        sizeof(conf->oth.batfile.before_process) - 1);
    SetTXMaxLen(fcgTXBatAfterPath,         sizeof(conf->oth.batfile.after_process) - 1);

    fcgTSTSettingsNotes->MaxLength     =   sizeof(conf->oth.notes) - 1;
}

System::Void frmConfig::InitStgFileList() {
    RebuildStgFileDropDown(String(sys_dat->exstg->s_local.stg_dir).ToString());
    stgChanged = false;
    CheckTSSettingsDropDownItem(nullptr);
}

System::Void frmConfig::fcgChangeEnabled(System::Object^  sender, System::EventArgs^  e) {
    fcgLBX265PathSub->Visible = fcgTSBCMDOnly->Checked;
    fcgTXX265PathSub->Visible = fcgTSBCMDOnly->Checked;
    fcgBTX265PathSub->Visible = fcgTSBCMDOnly->Checked;
    fcgBTCmdEx->Visible       = !fcgTSBCMDOnly->Checked;
    fcgCBNulOutCLI->Visible   = fcgTSBCMDOnly->Checked;
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

System::Void frmConfig::InitForm() {
    //UIテーマ切り替え
    CheckTheme();
    //言語設定ファイルのロード
    InitLangList();
    //fcgTabPageCtrlの初期化
    //fcgTabPageVideoController = gcnew TabPageCtrl(fcgtabControlVideo);
    //設定ファイル集の初期化
    InitStgFileList();
    //コンボボックスの値を設定
    InitComboBox();
    //タイマーの初期化
    InitTimer();
    //デフォルトの出力拡張子によってデフォルトのタブを変更
    switch (sys_dat->exstg->s_local.default_output_ext) {
    case 0: //mp4
        fcgtabControlMux->SelectedTab = fcgtabPageMP4;
        break;
    case 1: //mkv
    default:
        fcgtabControlMux->SelectedTab = fcgtabPageMKV;
        break;
    }
    //パラメータセット
    ConfToFrm(conf, true);
    //イベントセット
    SetTXMaxLenAll(); //テキストボックスの最大文字数
    SetAllCheckChangedEvents(this); //変更の確認,ついでにNUのフォームの変更可不可を更新
    fcgChangeMuxerVisible(nullptr, nullptr);
    fcgChangeEnabled(nullptr, nullptr);
    fcgCBAFS_CheckedChanged(nullptr, nullptr);
    EnableSettingsNoteChange(false);
    ExeTXPathLeave();
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



System::Void frmConfig::LoadLangText() {
    //一度ウィンドウの再描画を完全に抑止する
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 0, 0);
    //空白時にグレーで入れる文字列を言語変更のため一度空白に戻す
    ExeTXPathEnter();
    //言語更新開始
    LOAD_CLI_TEXT(fcgtabPageX265Main);
    LOAD_CLI_TEXT(fcgLBBitDepth);
    LOAD_CLI_TEXT(fcgLBPME);
    LOAD_CLI_TEXT(fcgLBPMode);
    LOAD_CLI_TEXT(fcgLBWpp);
    LOAD_CLI_TEXT(fcgLBFrameThreads);
    LOAD_CLI_TEXT(fcgLBThreads);
    LOAD_CLI_TEXT(fcgLBInterlaced);
    LOAD_CLI_TEXT(fcggroupBoxVUIColor);
    LOAD_CLI_TEXT(fcgLBFullRange);
    LOAD_CLI_TEXT(fcgLBTransfer);
    LOAD_CLI_TEXT(fcgLBColorPrim);
    LOAD_CLI_TEXT(fcgLBColorMatrix);
    LOAD_CLI_TEXT(fcgLBVideoFormat);
    LOAD_CLI_TEXT(fcgGroupBoxAspectRatio);
    LOAD_CLI_TEXT(fcgLBAspectRatio);
    LOAD_CLI_TEXT(fcgLBCSP);
    LOAD_CLI_TEXT(fcggroupBoxRateControl);
    LOAD_CLI_TEXT(fcgLBQComp);
    LOAD_CLI_TEXT(fcgLBVBVmax);
    LOAD_CLI_TEXT(fcgLBVBVbuf);
    LOAD_CLI_TEXT(fcggroupBoxPreset);
    LOAD_CLI_TEXT(fcgBTApplyPreset);
    LOAD_CLI_TEXT(fcgLBProfile);
    LOAD_CLI_TEXT(fcgLBX265Tune);
    LOAD_CLI_TEXT(fcgLBX265Preset);
    LOAD_CLI_TEXT(fcgLBSTATUS);
    LOAD_CLI_TEXT(fcgBTStatusFile);
    LOAD_CLI_TEXT(fcgLBQuality);
    LOAD_CLI_TEXT(fcgLBQualityLeft);
    LOAD_CLI_TEXT(fcgLBQualityRight);
    LOAD_CLI_TEXT(fcgCBNulOut);
    LOAD_CLI_TEXT(fcgCBAMPLimitBitrate);
    LOAD_CLI_TEXT(fcgCBFastFirstPass);
    LOAD_CLI_TEXT(fcgCBAMPLimitFileSize);
    LOAD_CLI_TEXT(fcgLBAutoNpass);
    LOAD_CLI_TEXT(fcgBTX265Path);
    LOAD_CLI_TEXT(fcgLBX265Path);
    LOAD_CLI_TEXT(fcgtabPageX265Other);
    LOAD_CLI_TEXT(fcgLBHEVCAQ);
    LOAD_CLI_TEXT(fcgLBSsimRd);
    LOAD_CLI_TEXT(fcgLBAQMotion);
    LOAD_CLI_TEXT(fcgLBQGSize);
    LOAD_CLI_TEXT(fcgLBRdoqLevel);
    LOAD_CLI_TEXT(fcgCBDeblock);
    LOAD_CLI_TEXT(fcgLBDeblockThreshold);
    LOAD_CLI_TEXT(fcgLBDeblockStrength);
    LOAD_CLI_TEXT(fcgLBPsyRDOQ);
    LOAD_CLI_TEXT(fcgLBPsyRD);
    LOAD_CLI_TEXT(fcgLBFade);
    LOAD_CLI_TEXT(fcgLBSceneCutMode);
    LOAD_CLI_TEXT(fcgLBWeightB);
    LOAD_CLI_TEXT(fcgLBScenecut);
    LOAD_CLI_TEXT(fcgLBRef);
    LOAD_CLI_TEXT(fcgLBBframes);
    LOAD_CLI_TEXT(fcgLBBadapt);
    LOAD_CLI_TEXT(fcgLBKeyintMax);
    LOAD_CLI_TEXT(fcgLBRcLookAhead);
    LOAD_CLI_TEXT(fcgLBWeightP);
    LOAD_CLI_TEXT(fcgLBBpyramid);
    LOAD_CLI_TEXT(fcgLBOpenGOP);
    LOAD_CLI_TEXT(fcgLBKeyintMin);
    LOAD_CLI_TEXT(fcgLBCUTree);
    LOAD_CLI_TEXT(fcgLBAQStrength);
    LOAD_CLI_TEXT(fcgLBAQMode);
    LOAD_CLI_TEXT(fcgLBRD);
    LOAD_CLI_TEXT(fcgLBRskipEdgeThreshold);
    LOAD_CLI_TEXT(fcgLBNoRecusrionSkip);
    LOAD_CLI_TEXT(fcgLBME);
    LOAD_CLI_TEXT(fcgLBSubME);
    LOAD_CLI_TEXT(fcgLBMERange);
    LOAD_CLI_TEXT(fcgLBMaxMerge);
    LOAD_CLI_TEXT(fcgLBLimitModes);
    LOAD_CLI_TEXT(fcgLBLimitRefs);
    LOAD_CLI_TEXT(fcgLBAsymmetricMP);
    LOAD_CLI_TEXT(fcgLBRectMP);
    LOAD_CLI_TEXT(fcgLBCtu);
    LOAD_CLI_TEXT(fcgLBTuInterDepth);
    LOAD_CLI_TEXT(fcgLBTuIntraDepth);
    LOAD_CLI_TEXT(fcgLBSAO);
    LOAD_CLI_TEXT(fcgtabPageX2652pass);
    LOAD_CLI_TEXT(fcgCBSVT);
    LOAD_CLI_TEXT(fcgCBAnalysisReuse);
    LOAD_CLI_TEXT(fcgTXAnalysisRefineInter);
    LOAD_CLI_TEXT(fcgTXAnalysisRefineIntra);
    LOAD_CLI_TEXT(fcgLBAnalysisReuseLevel);
    LOAD_CLI_TEXT(fcgLBAnalysisReuseFile);
    LOAD_CLI_TEXT(fcgBTAnalysisReuseFile);
    LOAD_CLI_TEXT(fcgtabPageExSettings);
    LOAD_CLI_TEXT(fcgCBSyncProcessAffinity);
    LOAD_CLI_TEXT(fcgLBX265PathSub);
    LOAD_CLI_TEXT(fcgBTX265PathSub);
    LOAD_CLI_TEXT(fcgLBTempDir);
    LOAD_CLI_TEXT(fcgBTCustomTempDir);
    LOAD_CLI_TEXT(fcggroupBoxCmdEx);
    LOAD_CLI_TEXT(fcgCBNulOutCLI);
    LOAD_CLI_TEXT(fcgBTCmdEx);
    LOAD_CLI_TEXT(fcgLBX265Priority);
    LOAD_CLI_TEXT(fcggroupBoxExSettings);
    LOAD_CLI_TEXT(fcgCBSetKeyframeAtChapter);
    LOAD_CLI_TEXT(fcgCBInputAsLW48);
    LOAD_CLI_TEXT(fcgCBCheckKeyframes);
    LOAD_CLI_TEXT(fcgCBAuoTcfileout);
    LOAD_CLI_TEXT(fcgCBAFSBitrateCorrection);
    LOAD_CLI_TEXT(fcgCBAFS);
    LOAD_CLI_TEXT(fcgtoolStripSettings);
    LOAD_CLI_TEXT(fcgTSBSave);
    LOAD_CLI_TEXT(fcgTSBSaveNew);
    LOAD_CLI_TEXT(fcgTSBDelete);
    LOAD_CLI_TEXT(fcgTSSettings);
    LOAD_CLI_TEXT(fcgTSBCMDOnly);
    LOAD_CLI_TEXT(fcgTSBBitrateCalc);
    LOAD_CLI_TEXT(fcgTSBOtherSettings);
    LOAD_CLI_TEXT(fcgTSLSettingsNotes);
    LOAD_CLI_TEXT(fcgTSTSettingsNotes);
    LOAD_CLI_TEXT(fcgtabPageMP4);
    LOAD_CLI_TEXT(fcgCBMP4MuxApple);
    LOAD_CLI_TEXT(fcgBTTC2MP4Path);
    LOAD_CLI_TEXT(fcgBTMP4MuxerPath);
    LOAD_CLI_TEXT(fcgLBTC2MP4Path);
    LOAD_CLI_TEXT(fcgLBMP4MuxerPath);
    LOAD_CLI_TEXT(fcgLBMP4CmdEx);
    LOAD_CLI_TEXT(fcgCBMP4MuxerExt);
    LOAD_CLI_TEXT(fcgBTMP4RawPath);
    LOAD_CLI_TEXT(fcgLBMP4RawPath);
    LOAD_CLI_TEXT(fcgBTMP4BoxTempDir);
    LOAD_CLI_TEXT(fcgLBMP4BoxTempDir);
    LOAD_CLI_TEXT(fcgtabPageMKV);
    LOAD_CLI_TEXT(fcgBTMKVMuxerPath);
    LOAD_CLI_TEXT(fcgLBMKVMuxerPath);
    LOAD_CLI_TEXT(fcgLBMKVMuxerCmdEx);
    LOAD_CLI_TEXT(fcgCBMKVMuxerExt);
    LOAD_CLI_TEXT(fcgtabPageMux);
    LOAD_CLI_TEXT(fcgLBMuxPriority);
    LOAD_CLI_TEXT(fcgtabPageBat);
    LOAD_CLI_TEXT(fcgLBBatAfterString);
    LOAD_CLI_TEXT(fcgLBBatBeforeString);
    LOAD_CLI_TEXT(fcgBTBatAfterPath);
    LOAD_CLI_TEXT(fcgLBBatAfterPath);
    LOAD_CLI_TEXT(fcgCBWaitForBatAfter);
    LOAD_CLI_TEXT(fcgCBRunBatAfter);
    LOAD_CLI_TEXT(fcgBTBatBeforePath);
    LOAD_CLI_TEXT(fcgLBBatBeforePath);
    LOAD_CLI_TEXT(fcgCBWaitForBatBefore);
    LOAD_CLI_TEXT(fcgCBRunBatBefore);
    LOAD_CLI_TEXT(fcgBTCancel);
    LOAD_CLI_TEXT(fcgBTOK);
    LOAD_CLI_TEXT(fcgBTDefault);
    LOAD_CLI_TEXT(fcgLBVersionDate);
    LOAD_CLI_TEXT(fcgLBVersion);
    LOAD_CLI_TEXT(fcgLBguiExBlog);
    LOAD_CLI_TEXT(fcgTSExeFileshelp);
    LOAD_CLI_TEXT(fcgtabPageAudioMain);
    LOAD_CLI_TEXT(fcgLBAudioDelayCut);
    LOAD_CLI_TEXT(fcgCBAudioEncTiming);
    LOAD_CLI_TEXT(fcgBTCustomAudioTempDir);
    LOAD_CLI_TEXT(fcgCBAudioUsePipe);
    LOAD_CLI_TEXT(fcgLBAudioBitrate);
    LOAD_CLI_TEXT(fcgCBAudio2pass);
    LOAD_CLI_TEXT(fcgLBAudioEncMode);
    LOAD_CLI_TEXT(fcgBTAudioEncoderPath);
    LOAD_CLI_TEXT(fcgLBAudioEncoderPath);
    LOAD_CLI_TEXT(fcgCBAudioOnly);
    LOAD_CLI_TEXT(fcgCBFAWCheck);
    LOAD_CLI_TEXT(fcgLBAudioEncoder);
    LOAD_CLI_TEXT(fcgLBAudioTemp);
    LOAD_CLI_TEXT(fcgtabPageAudioOther);
    LOAD_CLI_TEXT(fcgLBBatAfterAudioString);
    LOAD_CLI_TEXT(fcgLBBatBeforeAudioString);
    LOAD_CLI_TEXT(fcgBTBatAfterAudioPath);
    LOAD_CLI_TEXT(fcgLBBatAfterAudioPath);
    LOAD_CLI_TEXT(fcgCBRunBatAfterAudio);
    LOAD_CLI_TEXT(fcgBTBatBeforeAudioPath);
    LOAD_CLI_TEXT(fcgLBBatBeforeAudioPath);
    LOAD_CLI_TEXT(fcgCBRunBatBeforeAudio);
    LOAD_CLI_TEXT(fcgLBAudioPriority);
    //LOAD_CLI_MAIN_TEXT(fcgMain);

    //ローカル設定のロード(ini変更を反映)
    LoadLocalStg();
    //ローカル設定の反映
    SetLocalStg();
    //コンボボックスの値を設定
    InitComboBox();
    //ツールチップ
    SetHelpToolTips();
    SetX265VersionToolTip(LocalStg.x265Path);
    ActivateToolTip(sys_dat->exstg->s_local.disable_tooltip_help == FALSE);
    //タイムコードのappendix(後付修飾子)を反映
    fcgCBAuoTcfileout->Text = LOAD_CLI_STRING(AUO_CONFIG_TC_FILE_OUT) + L" (" + String(sys_dat->exstg->s_append.tc).ToString() + L")";
    { //タイトル表示,バージョン情報,コンパイル日時
        auto auo_full_name = g_auo_mes.get(AUO_GUIEX_FULL_NAME);
        if (auo_full_name == nullptr || wcslen(auo_full_name) == 0) auo_full_name = AUO_FULL_NAME_W;
        this->Text = String(auo_full_name).ToString();
        fcgLBVersion->Text = String(auo_full_name).ToString() + L" " + String(AUO_VERSION_STR_W).ToString();
        fcgLBVersionDate->Text = L"build " + String(__DATE__).ToString() + L" " + String(__TIME__).ToString();
    }
    //空白時にグレーで入れる文字列を言語に即して復活させる
    ExeTXPathLeave();
    //一度ウィンドウの再描画を再開し、強制的に再描画させる
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 1, 0);
    this->Refresh();
}

/////////////         データ <-> GUI     /////////////
System::Void frmConfig::ConfToFrm(CONF_GUIEX *cnf, bool all) {
    {
    //x265
    CONF_X265 *cx265 = &cnf->enc;
    memcpy(cnf_fcgTemp, cx265, sizeof(CONF_X265)); //一時保存用
    fcgCXBitDepth->SelectedIndex = get_bit_depth_idx(cx265->bit_depth);
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
    fcgCBAMPLimitBitrate->Checked  = (cnf->vid.amp_check & AMPLIMIT_BITRATE_UPPER) != 0;
    fcgCBAMPLimitFileSize->Checked = (cnf->vid.amp_check & AMPLIMIT_FILE_SIZE) != 0;
    SetNUValue(fcgNUAMPLimitFileSize, cnf->vid.amp_limit_file_size);
    SetNUValue(fcgNUAMPLimitBitrate,  cnf->vid.amp_limit_bitrate_upper);

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

    fcgCXSceneCutMode->SelectedIndex = cx265->hist_scenecut ? 1 : 0;
    SetNUValue(fcgNUScenecut,         cx265->scenecut);
    SetNUValue(fcgNUHistThreshold,    cx265->hist_threshold);
    SetNUValue(fcgNUKeyintMin,        cx265->keyint_min);
    SetNUValue(fcgNUKeyintMax,        cx265->keyint_max);
    fcgCBFade->Checked              = cx265->fades != 0;
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
    SetNUValue(fcgNUQComp,            cx265->qp_compress * 100);

    SetNUValue(fcgNURD,               cx265->rd);
    SetCXIndex(fcgCXAQMode,           cx265->aq_mode);
    SetNUValue(fcgNUAQStrength,       cx265->aq_strength);
    fcgCBAQMotion->Checked          = cx265->aq_motion != 0;
    fcgCBHEVCAQ->Checked            = cx265->hevc_aq != 0;
    SetNUValue(fcgNUPsyRD,            cx265->psy_rd.x);
    SetNUValue(fcgNUPsyRDOQ,          cx265->psy_rdoq);
    SetNUValue(fcgNURdoqLevel,        cx265->rdoq_level);
    fcgCBCUTree->Checked            = cx265->cutree != 0;
    SetNUValue(fcgNUQGSize,           cx265->qg_size);
    fcgCBSsimRd->Checked            = cx265->ssim_rd != 0;

    SetNUValue(fcgNUCtu,              cx265->ctu);
    SetNUValue(fcgNUTuIntraDepth,     cx265->tu_intra_depth);
    SetNUValue(fcgNUTuInterDepth,     cx265->tu_inter_depth);
    fcgCBRectMP->Checked            = cx265->rect_mp != 0;
    fcgCBAsymmetricMP->Checked      = cx265->asymmnteric_mp != 0;
    SetNUValue(fcgNULimitRefs,        cx265->limit_refs);
    fcgCBLimitModes->Checked        = cx265->limit_modes != 0;

    SetCXIndex(fcgCXME,               cx265->me);
    SetCXIndex(fcgCXSubME,            cx265->subme);
    SetNUValue(fcgNUMERange,          cx265->me_range);
    SetNUValue(fcgNUMaxMerge,         cx265->max_merge);
    SetNUValue(fcgNURskip,            cx265->recursion_skip);
    SetNUValue(fcgNURskipEdgeThreshold, cx265->rskip_edge_threshold);

    SetNUValue(fcgNUPools,            cx265->pools);
    SetNUValue(fcgNUFrameThreads,     cx265->frame_threads);
    fcgCBWpp->Checked               = cx265->wpp != 0;
    fcgCBPMode->Checked             = cx265->pmode != 0;
    fcgCBPME->Checked               = cx265->pme != 0;

    fcgCBDeblock->Checked           = cx265->use_deblock != 0;
    SetNUValue(fcgNUDeblockStrength,  cx265->deblock.x);
    SetNUValue(fcgNUDeblockThreshold, cx265->deblock.y);
    fcgCBSAO->Checked               = cx265->sao != 0;

    fcgCBSVT->Checked               = cx265->svt != 0;

    fcgCBAnalysisReuse->Checked     = cx265->analysis_reuse != 0;
    SetNUValue(fcgNUAnalysisReuseLevel, cx265->analysis_reuse_level);
    SetNUValue(fcgNURefineInter,        cx265->refine_inter);
    SetNUValue(fcgNURefineIntra,        cx265->refine_intra);

    fcgTXAnalysisReuseFile->Text    = (str_has_char(cnf->vid.analysis_file)) ? String(cnf->vid.analysis_file).ToString() : String(DefaultAnalysisFilePath).ToString();
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
        fcgCBSyncProcessAffinity->Checked  = cnf->vid.sync_process_affinity != 0;

        SetCXIndex(fcgCXX264Priority,        cnf->vid.priority);
        SetCXIndex(fcgCXTempDir,             cnf->oth.temp_dir);

        fcgTXCmdEx->Text            = String(cnf->vid.cmdex).ToString();
        if (cnf->oth.disable_guicmd)
            fcgCBNulOutCLI->Checked        = cnf->enc.nul_out != 0;

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
        fcgCBRunBatBeforeAudio->Checked    =(cnf->oth.run_bat & RUN_BAT_BEFORE_AUDIO) != 0;
        fcgCBRunBatAfterAudio->Checked     =(cnf->oth.run_bat & RUN_BAT_AFTER_AUDIO) != 0;
        fcgTXBatBeforeAudioPath->Text      = String(cnf->oth.batfile.before_audio).ToString();
        fcgTXBatAfterAudioPath->Text       = String(cnf->oth.batfile.after_audio).ToString();

        //mux
        fcgCBMP4MuxerExt->Checked          = cnf->mux.disable_mp4ext == 0;
        fcgCBMP4MuxApple->Checked          = cnf->mux.apple_mode != 0;
        SetCXIndex(fcgCXMP4CmdEx,            cnf->mux.mp4_mode);
        SetCXIndex(fcgCXMP4BoxTempDir,       cnf->mux.mp4_temp_dir);
        fcgCBMKVMuxerExt->Checked          = cnf->mux.disable_mkvext == 0;
        SetCXIndex(fcgCXMKVCmdEx,            cnf->mux.mkv_mode);
        SetCXIndex(fcgCXMuxPriority,         cnf->mux.priority);

        fcgCBRunBatBefore->Checked         =(cnf->oth.run_bat & RUN_BAT_BEFORE_PROCESS) != 0;
        fcgCBRunBatAfter->Checked          =(cnf->oth.run_bat & RUN_BAT_AFTER_PROCESS)  != 0;
        fcgCBWaitForBatBefore->Checked     =(cnf->oth.dont_wait_bat_fin & RUN_BAT_BEFORE_PROCESS) == 0;
        fcgCBWaitForBatAfter->Checked      =(cnf->oth.dont_wait_bat_fin & RUN_BAT_AFTER_PROCESS)  == 0;
        fcgTXBatBeforePath->Text           = String(cnf->oth.batfile.before_process).ToString();
        fcgTXBatAfterPath->Text            = String(cnf->oth.batfile.after_process).ToString();

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
    cnf->enc.bit_depth            = get_bit_depth(fcgCXBitDepth->SelectedIndex);
    cnf->enc.rc_mode              = cnf_fcgTemp->rc_mode;
    cnf->enc.bitrate              = cnf_fcgTemp->bitrate;
    cnf->enc.qp                   = cnf_fcgTemp->qp;
    cnf->enc.crf                  = cnf_fcgTemp->crf;
    cnf->enc.nul_out              = fcgCBNulOut->Checked;
    cnf->enc.pass                 = cnf_fcgTemp->pass;
    cnf->enc.slow_first_pass      = cnf_fcgTemp->slow_first_pass;
    cnf->enc.use_auto_npass       = cnf_fcgTemp->use_auto_npass;
    cnf->enc.auto_npass           = (int)fcgNUAutoNPass->Value;
    cnf->vid.amp_check             = NULL;
    cnf->vid.amp_check            |= fcgCBAMPLimitBitrate->Checked ? AMPLIMIT_BITRATE_UPPER : NULL;
    cnf->vid.amp_check            |= fcgCBAMPLimitFileSize->Checked ? AMPLIMIT_FILE_SIZE : NULL;
    cnf->vid.amp_limit_bitrate_upper     = (double)fcgNUAMPLimitBitrate->Value;
    cnf->vid.amp_limit_file_size   = (double)fcgNUAMPLimitFileSize->Value;
    cnf->enc.preset               = fcgCXPreset->SelectedIndex;
    cnf->enc.tune                 = fcgCXTune->SelectedIndex;
    cnf->enc.profile              = fcgCXProfile->SelectedIndex;

    cnf->enc.sar.x                = (int)fcgNUAspectRatioX->Value * ((fcgCXAspectRatio->SelectedIndex != 1) ? 1 : -1);
    cnf->enc.sar.y                = (int)fcgNUAspectRatioY->Value * ((fcgCXAspectRatio->SelectedIndex != 1) ? 1 : -1);
    cnf->enc.interlaced           = fcgCXInterlaced->SelectedIndex;

    cnf->enc.videoformat          = fcgCXVideoFormat->SelectedIndex;
    cnf->enc.output_csp           = fcgCXCSP->SelectedIndex;

    cnf->enc.colormatrix          = fcgCXColorMatrix->SelectedIndex;
    cnf->enc.colorprim            = fcgCXColorPrim->SelectedIndex;
    cnf->enc.transfer             = fcgCXTransfer->SelectedIndex;
    cnf->enc.input_range          = fcgCBFullRange->Checked;

    cnf->enc.hist_scenecut        = fcgCXSceneCutMode->SelectedIndex > 0 ? TRUE : FALSE;
    cnf->enc.scenecut             = (int)fcgNUScenecut->Value;
    cnf->enc.hist_threshold       = (float)fcgNUHistThreshold->Value;
    cnf->enc.keyint_min           = (int)fcgNUKeyintMin->Value;
    cnf->enc.keyint_max           = (int)fcgNUKeyintMax->Value;
    cnf->enc.fades                = fcgCBFade->Checked;
    cnf->enc.open_gop             = fcgCBOpenGOP->Checked;
    cnf->enc.rc_lookahead         = (int)fcgNURCLookahead->Value;
    cnf->enc.ref_frames           = (int)fcgNURef->Value;
    cnf->enc.bframes              = (int)fcgNUBframes->Value;
    cnf->enc.b_adapt              = fcgCXBadapt->SelectedIndex;
    cnf->enc.b_pyramid            = fcgCBBpyramid->Checked;
    cnf->enc.weight_b             = fcgCBWeightB->Checked;
    cnf->enc.weight_p             = fcgCBWeightP->Checked;

    cnf->enc.vbv_bufsize          = (int)fcgNUVBVbuf->Value;
    cnf->enc.vbv_maxrate          = (int)fcgNUVBVmax->Value;
    cnf->enc.qp_compress          = (float)(fcgNUQComp->Value * (System::Decimal)0.01);

    cnf->enc.rd                   = (int)fcgNURD->Value;
    cnf->enc.aq_mode              = fcgCXAQMode->SelectedIndex;
    cnf->enc.aq_strength          = (float)fcgNUAQStrength->Value;
    cnf->enc.aq_motion            = fcgCBAQMotion->Checked;
    cnf->enc.hevc_aq              = fcgCBHEVCAQ->Checked;
    cnf->enc.psy_rd.x             = (float)fcgNUPsyRD->Value;
    cnf->enc.psy_rdoq             = (float)fcgNUPsyRDOQ->Value;
    cnf->enc.rdoq_level           = (int)fcgNURdoqLevel->Value;
    cnf->enc.ssim_rd              = fcgCBSsimRd->Checked;
    cnf->enc.cutree               = fcgCBCUTree->Checked;
    cnf->enc.qg_size              = (int)fcgNUQGSize->Value;

    cnf->enc.ctu                  = (int)fcgNUCtu->Value;
    cnf->enc.tu_intra_depth       = (int)fcgNUTuIntraDepth->Value;
    cnf->enc.tu_inter_depth       = (int)fcgNUTuInterDepth->Value;
    cnf->enc.rect_mp              = fcgCBRectMP->Checked;
    cnf->enc.asymmnteric_mp       = fcgCBAsymmetricMP->Checked;
    cnf->enc.limit_refs           = (int)fcgNULimitRefs->Value;
    cnf->enc.limit_modes          = fcgCBLimitModes->Checked;

    cnf->enc.me                   = fcgCXME->SelectedIndex;
    cnf->enc.subme                = fcgCXSubME->SelectedIndex;
    cnf->enc.me_range             = (int)fcgNUMERange->Value;
    cnf->enc.max_merge            = (int)fcgNUMaxMerge->Value;
    cnf->enc.recursion_skip       = (int)fcgNURskip->Value;
    cnf->enc.rskip_edge_threshold = (int)fcgNURskipEdgeThreshold->Value;

    cnf->enc.pools                = (int)fcgNUPools->Value;
    cnf->enc.frame_threads        = (int)fcgNUFrameThreads->Value;
    cnf->enc.wpp                  = fcgCBWpp->Checked;
    cnf->enc.pmode                = fcgCBPMode->Checked;
    cnf->enc.pme                  = fcgCBPME->Checked;

    cnf->enc.use_deblock          = fcgCBDeblock->Checked;
    cnf->enc.deblock.x            = (int)fcgNUDeblockStrength->Value;
    cnf->enc.deblock.y            = (int)fcgNUDeblockThreshold->Value;
    cnf->enc.sao                  = fcgCBSAO->Checked;

    cnf->enc.ip_ratio             = 0.0f;
    cnf->enc.pb_ratio             = 0.0f;

    GetCHARfromString(cnf->vid.stats, fcgTXStatusFile->Text);

    cnf->enc.svt                  = fcgCBSVT->Checked;

    cnf->enc.analysis_reuse       = fcgCBAnalysisReuse->Checked;
    cnf->enc.analysis_reuse_level = (int)fcgNUAnalysisReuseLevel->Value;
    cnf->enc.refine_intra         = (int)fcgNURefineIntra->Value;
    cnf->enc.refine_inter         = (int)fcgNURefineInter->Value;

    GetCHARfromString(cnf->vid.analysis_file, fcgTXAnalysisReuseFile->Text);

    //拡張部
    cnf->vid.afs                    = fcgCBAFS->Checked;
    cnf->vid.afs_bitrate_correction = fcgCBAFSBitrateCorrection->Checked;
    cnf->vid.auo_tcfile_out         = fcgCBAuoTcfileout->Checked;
    cnf->vid.check_keyframe         = CHECK_KEYFRAME_NONE;
    cnf->vid.check_keyframe        |= (fcgCBCheckKeyframes->Checked)       ? CHECK_KEYFRAME_AVIUTL  : CHECK_KEYFRAME_NONE;
    cnf->vid.check_keyframe        |= (fcgCBSetKeyframeAtChapter->Checked) ? CHECK_KEYFRAME_CHAPTER : CHECK_KEYFRAME_NONE;
    cnf->vid.priority               = fcgCXX264Priority->SelectedIndex;
    cnf->vid.input_as_lw48          = fcgCBInputAsLW48->Checked;
    cnf->vid.sync_process_affinity  = fcgCBSyncProcessAffinity->Checked;
    cnf->oth.temp_dir               = fcgCXTempDir->SelectedIndex;
    GetCHARfromString(cnf->vid.cmdex, fcgTXCmdEx->Text);

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
    cnf->mux.priority               = fcgCXMuxPriority->SelectedIndex;

    cnf->oth.run_bat                = RUN_BAT_NONE;
    cnf->oth.run_bat               |= (fcgCBRunBatBeforeAudio->Checked) ? RUN_BAT_BEFORE_AUDIO   : NULL;
    cnf->oth.run_bat               |= (fcgCBRunBatAfterAudio->Checked)  ? RUN_BAT_AFTER_AUDIO    : NULL;
    cnf->oth.run_bat               |= (fcgCBRunBatBefore->Checked)      ? RUN_BAT_BEFORE_PROCESS : NULL;
    cnf->oth.run_bat               |= (fcgCBRunBatAfter->Checked)       ? RUN_BAT_AFTER_PROCESS  : NULL;
    cnf->oth.dont_wait_bat_fin      = RUN_BAT_NONE;
    cnf->oth.dont_wait_bat_fin     |= (!fcgCBWaitForBatBefore->Checked) ? RUN_BAT_BEFORE_PROCESS : NULL;
    cnf->oth.dont_wait_bat_fin     |= (!fcgCBWaitForBatAfter->Checked)  ? RUN_BAT_AFTER_PROCESS  : NULL;
    GetCHARfromString(cnf->oth.batfile.before_process, fcgTXBatBeforePath->Text);
    GetCHARfromString(cnf->oth.batfile.after_process,  fcgTXBatAfterPath->Text);
    GetCHARfromString(cnf->oth.batfile.before_audio,   fcgTXBatBeforeAudioPath->Text);
    GetCHARfromString(cnf->oth.batfile.after_audio,    fcgTXBatAfterAudioPath->Text);

    GetfcgTSLSettingsNotes(cnf->oth.notes, sizeof(cnf->oth.notes));

    //cli mode
    cnf->oth.disable_guicmd         = fcgTSBCMDOnly->Checked;
    if (cnf->oth.disable_guicmd) {
        cnf->enc.nul_out           = fcgCBNulOutCLI->Checked;
    }

    //制約条件として適用
    set_profile_to_conf(&cnf->enc, cnf->enc.profile);
}

System::Void frmConfig::GetfcgTSLSettingsNotes(char *notes, int nSize) {
    ZeroMemory(notes, nSize);
    if (fcgTSLSettingsNotes->Overflow != ToolStripItemOverflow::Never)
        GetCHARfromString(notes, nSize, fcgTSLSettingsNotes->Text);
}

System::Void frmConfig::SetfcgTSLSettingsNotes(const char *notes) {
    if (str_has_char(notes)) {
        fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[0][0], StgNotesColor[0][1], StgNotesColor[0][2]);
        fcgTSLSettingsNotes->Text = String(notes).ToString();
        fcgTSLSettingsNotes->Overflow = ToolStripItemOverflow::AsNeeded;
    } else {
        fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[1][0], StgNotesColor[1][1], StgNotesColor[1][2]);
        fcgTSLSettingsNotes->Text = LOAD_CLI_STRING(AuofcgTSTSettingsNotes);
        fcgTSLSettingsNotes->Overflow = ToolStripItemOverflow::Never;
    }
}

System::Void frmConfig::SetfcgTSLSettingsNotes(String^ notes) {
    if (notes->Length && fcgTSLSettingsNotes->Overflow != ToolStripItemOverflow::Never) {
        fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[0][0], StgNotesColor[0][1], StgNotesColor[0][2]);
        fcgTSLSettingsNotes->Text = notes;
        fcgTSLSettingsNotes->Overflow = ToolStripItemOverflow::AsNeeded;
    } else {
        fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[1][0], StgNotesColor[1][1], StgNotesColor[1][2]);
        fcgTSLSettingsNotes->Text = LOAD_CLI_STRING(AuofcgTSTSettingsNotes);
        fcgTSLSettingsNotes->Overflow = ToolStripItemOverflow::Never;
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

System::Void frmConfig::TabControl_DarkDrawItem(System::Object^ sender, DrawItemEventArgs^ e) {
    //対象のTabControlを取得
    TabControl^ tab = dynamic_cast<TabControl^>(sender);
    //タブページのテキストを取得
    System::String^ txt = tab->TabPages[e->Index]->Text;

    //タブのテキストと背景を描画するためのブラシを決定する
    SolidBrush^ foreBrush = gcnew System::Drawing::SolidBrush(ColorfromInt(DEFAULT_UI_COLOR_TEXT_DARK));
    SolidBrush^ backBrush = gcnew System::Drawing::SolidBrush(ColorfromInt(DEFAULT_UI_COLOR_BASE_DARK));

    //StringFormatを作成
    StringFormat^ sf = gcnew System::Drawing::StringFormat();
    //中央に表示する
    sf->Alignment = StringAlignment::Center;
    sf->LineAlignment = StringAlignment::Center;

    //背景の描画
    e->Graphics->FillRectangle(backBrush, e->Bounds);
    //Textの描画
    e->Graphics->DrawString(txt, e->Font, foreBrush, e->Bounds, sf);
}

System::Void frmConfig::fcgMouseEnter_SetColor(System::Object^  sender, System::EventArgs^  e) {
    fcgMouseEnterLeave_SetColor(sender, themeMode, DarkenWindowState::Hot, dwStgReader);
}
System::Void frmConfig::fcgMouseLeave_SetColor(System::Object^  sender, System::EventArgs^  e) {
    fcgMouseEnterLeave_SetColor(sender, themeMode, DarkenWindowState::Normal, dwStgReader);
}

System::Void frmConfig::SetAllMouseMove(Control ^top, const AuoTheme themeTo) {
    if (themeTo == themeMode) return;
    System::Type^ type = top->GetType();
    if (type == CheckBox::typeid /* || isToolStripItem(type)*/) {
        top->MouseEnter += gcnew System::EventHandler(this, &frmConfig::fcgMouseEnter_SetColor);
        top->MouseLeave += gcnew System::EventHandler(this, &frmConfig::fcgMouseLeave_SetColor);
    } else if (type == ToolStrip::typeid) {
        ToolStrip^ TS = dynamic_cast<ToolStrip^>(top);
        for (int i = 0; i < TS->Items->Count; i++) {
            auto item = TS->Items[i];
            item->MouseEnter += gcnew System::EventHandler(this, &frmConfig::fcgMouseEnter_SetColor);
            item->MouseLeave += gcnew System::EventHandler(this, &frmConfig::fcgMouseLeave_SetColor);
        }
    }
    for (int i = 0; i < top->Controls->Count; i++) {
        SetAllMouseMove(top->Controls[i], themeTo);
    }
}

System::Void frmConfig::CheckTheme() {
    //DarkenWindowが使用されていれば設定をロードする
    if (dwStgReader != nullptr) delete dwStgReader;
    const auto [themeTo, dwStg] = check_current_theme(sys_dat->aviutl_dir);
    dwStgReader = dwStg;

    //変更の必要がなければ終了
    if (themeTo == themeMode) return;

    //一度ウィンドウの再描画を完全に抑止する
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 0, 0);
#if 1
    //tabcontrolのborderを隠す
    SwitchComboBoxBorder(fcgtabControlVideo, fcgPNHideTabControlVideo, themeMode, themeTo, dwStgReader);
    SwitchComboBoxBorder(fcgtabControlAudio, fcgPNHideTabControlAudio, themeMode, themeTo, dwStgReader);
    SwitchComboBoxBorder(fcgtabControlMux,   fcgPNHideTabControlMux,   themeMode, themeTo, dwStgReader);
    //上部のtoolstripborderを隠すためのパネル
    fcgPNHideToolStripBorder->Visible = themeTo == AuoTheme::DarkenWindowDark;
#endif
#if 0
    //TabControlをオーナードローする
    fcgtabControlVideo->DrawMode = TabDrawMode::OwnerDrawFixed;
    fcgtabControlVideo->DrawItem += gcnew DrawItemEventHandler(this, &frmConfig::TabControl_DarkDrawItem);

    fcgtabControlAudio->DrawMode = TabDrawMode::OwnerDrawFixed;
    fcgtabControlAudio->DrawItem += gcnew DrawItemEventHandler(this, &frmConfig::TabControl_DarkDrawItem);

    fcgtabControlMux->DrawMode = TabDrawMode::OwnerDrawFixed;
    fcgtabControlMux->DrawItem += gcnew DrawItemEventHandler(this, &frmConfig::TabControl_DarkDrawItem);
#endif
    if (themeTo != themeMode) {
        SetAllColor(this, themeTo, this->GetType(), dwStgReader);
        SetAllMouseMove(this, themeTo);
    }
    //一度ウィンドウの再描画を再開し、強制的に再描画させる
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 1, 0);
    this->Refresh();
    themeMode = themeTo;
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
    const ENC_OPTION_STR *list = get_option_list_x265(type);
    fcgTTX265->SetToolTip(control, L"--" + String(type).ToString() + L"\n"
        + LOAD_CLI_STRING(AuofrmTTColorMatrix1) + L"\n"
        + LOAD_CLI_STRING(AuofrmTTColorMatrix2) + L"\n"
        + LOAD_CLI_STRING(AuofrmTTColorMatrix3) + L" " + COLOR_MATRIX_THRESHOLD + L" " + LOAD_CLI_STRING(AuofrmTTColorMatrix4) + L" … " + String(list[COLOR_MATRIX_HD].desc).ToString() + L"\n"
        + LOAD_CLI_STRING(AuofrmTTColorMatrix3) + L" " + COLOR_MATRIX_THRESHOLD + L" " + LOAD_CLI_STRING(AuofrmTTColorMatrix5) + L" … " + String(list[COLOR_MATRIX_SD].desc).ToString() + L"\n"
        + LOAD_CLI_STRING(AuofrmTTColorMatrix6)
    );
}

System::Void frmConfig::SetHelpToolTips() {
    //x265
#define SET_TOOL_TIP_X265(target, x) { fcgTTX265->SetToolTip(target, LOAD_CLI_STRING(AuofrmTT ## x)); }
#define SET_TOOL_TIP_EX2(target, x) { fcgTTEx->SetToolTip(target, LOAD_CLI_STRING(AuofrmTT ## x)); }
#define SET_TOOL_TIP_EX(target) { fcgTTEx->SetToolTip(target, LOAD_CLI_STRING(AuofrmTT ## target)); }


    SET_TOOL_TIP_X265(fcgBTX265Path, fcgBTX265Path);
    SET_TOOL_TIP_X265(fcgBTX265PathSub, fcgBTX265Path);
    fcgTTX265->SetToolTip(fcgCXX265Mode, L""
        + L"【" + LOAD_CLI_STRING(AuofrmTTfcgCXX265ModeSinglePath) + L"】\n"
        + L"   " + LOAD_CLI_STRING(x265_encodemode_desc[2].mes)->Replace(LOAD_CLI_STRING(AuofrmTTfcgCXX265ModeSinglePath) + L" - ", L"") + L"\t … --crf\n"
        + L"   " + LOAD_CLI_STRING(x265_encodemode_desc[1].mes)->Replace(LOAD_CLI_STRING(AuofrmTTfcgCXX265ModeSinglePath) + L" - ", L"") + L"\t\t … --qp\n"
        + L"   " + LOAD_CLI_STRING(x265_encodemode_desc[0].mes)->Replace(LOAD_CLI_STRING(AuofrmTTfcgCXX265ModeSinglePath) + L" - ", L"") + L"\t … --bitrate\n"
        + L"\n"
        + L"【" + LOAD_CLI_STRING(AuofrmTTfcgCXX265ModeMultiPath) + L"】\n"
        + L"   " + LOAD_CLI_STRING(x265_encodemode_desc[3].mes)->Replace(LOAD_CLI_STRING(AuofrmTTfcgCXX265ModeMultiPath) + L" - ", L"") + L"\t … --pass 1 --bitrate\n"
        + L"   " + LOAD_CLI_STRING(x265_encodemode_desc[4].mes)->Replace(LOAD_CLI_STRING(AuofrmTTfcgCXX265ModeMultiPath) + L" - ", L"") + L"\t … --pass 3 --bitrate\n"
        + L"\n"
        + L"【" + LOAD_CLI_STRING(x265_encodemode_desc[5].mes) + L"】\n"
        + L"    " + LOAD_CLI_STRING(AuofrmTTfcgCXX265ModeAutoMultiPath) + L"\n"
        + L"    --pass 1/3 --bitrate\n"
        + L"\n"
        + L"【" + LOAD_CLI_STRING(x265_encodemode_desc[6].mes) + L"】\n"
        + L"    " + LOAD_CLI_STRING(AuofrmTTfcgCXX265ModeCrfWithCheck) + L"\n"
        + L"    --crf"
        );

    fcgTTX265->SetToolTip(fcgCBNulOut,            L"-o nul");
    fcgTTX265->SetToolTip(fcgCBFastFirstPass,     LOAD_CLI_STRING(AuofrmTTWhenCheckOff) + L"--slow-firstpass");
    fcgTTX265->SetToolTip(fcgTXQuality,           L"--crf / --bitrate / --qp");
    fcgTTX265->SetToolTip(fcgTXStatusFile,        L"--stats");
    fcgTTX265->SetToolTip(fcgCXProfile,           L"--profile\n"
        + L"\n"
        + LOAD_CLI_STRING(AuofrmTTfcgCXProfile)
        );

    fcgTTX265->SetToolTip(fcgCBNulOut,            L"-o nul");
    fcgTTX265->SetToolTip(fcgCBFastFirstPass,     LOAD_CLI_STRING(AuofrmTTWhenCheckOff) + L"--slow-firstpass");
    fcgTTX265->SetToolTip(fcgTXQuality,           L"--crf / --bitrate / --qp");
    fcgTTX265->SetToolTip(fcgTXStatusFile,        L"--stats");
    fcgTTX265->SetToolTip(fcgCXProfile,           L"--profile\n"
        + L"\n"
        + LOAD_CLI_STRING(AuofrmTTfcgCXProfile)
        );

    //自動マルチパス 上限設定
    SET_TOOL_TIP_EX2(fcgCBAMPLimitFileSize, fcgAMPLimitFileSize);
    SET_TOOL_TIP_EX2(fcgNUAMPLimitFileSize, fcgAMPLimitFileSize);

    //プロファイルとか
    fcgTTX265->SetToolTip(fcgCXTune,              L"--tune");
    fcgTTX265->SetToolTip(fcgCXPreset,            L"--preset");
    SET_TOOL_TIP_EX(fcgBTApplyPreset);

    fcgTTX265->SetToolTip(fcgCXAspectRatio, L""
        + LOAD_CLI_STRING(aspect_desc[0].mes) + L"\n"
        + L"   " + LOAD_CLI_STRING(AuofrmTTfcgCXAspectRatioSAR) + L"\n"
        + L"\n"
        + LOAD_CLI_STRING(aspect_desc[1].mes) + L"\n"
        + L"   " + LOAD_CLI_STRING(AuofrmTTfcgCXAspectRatioDAR) + L"\n"
    );
    SET_TOOL_TIP_X265(fcgNUAspectRatioX, fcgNUAspectRatioX);
    SET_TOOL_TIP_X265(fcgNUAspectRatioY, fcgNUAspectRatioY);

    fcgTTX265->SetToolTip(fcgCXInterlaced,       L"--interlace");
    fcgTTX265->SetToolTip(fcgCXVideoFormat,      L"--videoformat");

    fcgTTX265->SetToolTip(fcgCXBitDepth,         L"--input-depth / --output-depth");
    fcgTTX265->SetToolTip(fcgCXCSP,              L"--input-csp\n"
        + LOAD_CLI_STRING(AuofrmTTfcgCXOutputCsp)
        );
    SetHelpToolTipsColorMatrixX265(fcgCXColorMatrix, "colormatrix");
    SetHelpToolTipsColorMatrixX265(fcgCXColorPrim,   "colorprim");
    SetHelpToolTipsColorMatrixX265(fcgCXTransfer,    "transfer");
    fcgTTX265->SetToolTip(fcgCBFullRange,        L"--range");

    fcgTTX265->SetToolTip(fcgNUVBVbuf,           L"--vbv-bufsize");
    fcgTTX265->SetToolTip(fcgNUVBVmax,           L"--vbv-maxrate");
    fcgTTX265->SetToolTip(fcgNUQComp,            L"--qcomp");
    fcgTTX265->SetToolTip(fcgCXSceneCutMode,     L"--hist-scenecut / --no-hist-scenecut");
    fcgTTX265->SetToolTip(fcgNUScenecut,         L"--scenecut");
    fcgTTX265->SetToolTip(fcgNUHistThreshold,    L"--hist-threshold");
    fcgTTX265->SetToolTip(fcgCBFade,             L"--fades");
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
    fcgTTX265->SetToolTip(fcgCBAQMotion,         L"--aq-motion");
    fcgTTX265->SetToolTip(fcgNUPsyRD,            L"--psy-rd");
    fcgTTX265->SetToolTip(fcgNUPsyRDOQ,          L"--psy-rdoq");
    fcgTTX265->SetToolTip(fcgNURdoqLevel,        L"--rdoq-level");
    fcgTTX265->SetToolTip(fcgCBCUTree,           L"--cutree");
    fcgTTX265->SetToolTip(fcgNUQGSize,           L"--qg-size");
    fcgTTX265->SetToolTip(fcgCBSsimRd,           L"--ssim-rd");

    fcgTTX265->SetToolTip(fcgNUCtu,              L"--ctu");
    fcgTTX265->SetToolTip(fcgNUTuIntraDepth,     L"--tu-intra-depth");
    fcgTTX265->SetToolTip(fcgNUTuInterDepth,     L"--tu-inter-depth");
    fcgTTX265->SetToolTip(fcgCBAsymmetricMP,     L"--amp");
    fcgTTX265->SetToolTip(fcgCBRectMP,           L"--rect");
    fcgTTX265->SetToolTip(fcgNULimitRefs,        L"--limit-refs");
    fcgTTX265->SetToolTip(fcgCBLimitModes,       L"--limit-modes");

    fcgTTX265->SetToolTip(fcgCXME,               L"--me");
    fcgTTX265->SetToolTip(fcgCXSubME,            L"--subme");
    fcgTTX265->SetToolTip(fcgNUMERange,          L"--merange");
    fcgTTX265->SetToolTip(fcgNUMaxMerge,         L"--max-merge");
    fcgTTX265->SetToolTip(fcgNURskip,            L"--rskip");
    fcgTTX265->SetToolTip(fcgNURskipEdgeThreshold, L"--rskip-edge-threshold");

    fcgTTX265->SetToolTip(fcgNUPools,            L"--pools\n"
        + LOAD_CLI_STRING(AuofrmTTZeroAsAuto)
        );
    fcgTTX265->SetToolTip(fcgNUFrameThreads,     L"--frame-threads");
    fcgTTX265->SetToolTip(fcgCBWpp,              L"--wpp");
    fcgTTX265->SetToolTip(fcgCBPMode,            L"--pmode");
    fcgTTX265->SetToolTip(fcgCBPME,              L"--pme");

    fcgTTX265->SetToolTip(fcgCBSAO,              L"--sao");
    fcgTTX265->SetToolTip(fcgCBDeblock,          L"--deblock <Strength>:<Threshold>\n" + LOAD_CLI_STRING(AuofrmTTWhenCheckOff) + L" --no-deblock");
    fcgTTX265->SetToolTip(fcgNUDeblockStrength,  L"--deblock <Strength>:<Threshold>");
    fcgTTX265->SetToolTip(fcgNUDeblockThreshold, L"--deblock <Strength>:<Threshold>");

    fcgTTX265->SetToolTip(fcgCBAnalysisReuse,      L"--analysis-reuse-mode");
    fcgTTX265->SetToolTip(fcgNUAnalysisReuseLevel, L"--analysis-reuse-level");
    fcgTTX265->SetToolTip(fcgTXAnalysisReuseFile,  L"--analysis-reuse-file");
    fcgTTX265->SetToolTip(fcgNURefineIntra,        L"--refine-intra");
    fcgTTX265->SetToolTip(fcgNURefineInter,        L"--refine-inter");

    fcgTTX265->SetToolTip(fcgCBSVT,                L"--svt");

    //拡張
    SET_TOOL_TIP_EX(fcgCBAFS);
    SET_TOOL_TIP_EX(fcgCBAFSBitrateCorrection);
    SET_TOOL_TIP_EX(fcgCBAuoTcfileout);
    SET_TOOL_TIP_EX(fcgCBCheckKeyframes);
    SET_TOOL_TIP_EX(fcgCBSetKeyframeAtChapter);
    SET_TOOL_TIP_EX(fcgCBInputAsLW48);
    SET_TOOL_TIP_EX(fcgCXTempDir);
    SET_TOOL_TIP_EX(fcgBTCustomTempDir);
    SET_TOOL_TIP_EX(fcgBTCmdEx);

    //音声
    SET_TOOL_TIP_EX(fcgCXAudioEncoder);
    SET_TOOL_TIP_EX(fcgCBAudioOnly);
    SET_TOOL_TIP_EX(fcgCBFAWCheck);
    SET_TOOL_TIP_EX(fcgBTAudioEncoderPath);
    SET_TOOL_TIP_EX(fcgCXAudioEncMode);
    SET_TOOL_TIP_EX(fcgCBAudio2pass);
    SET_TOOL_TIP_EX(fcgCBAudioUsePipe);
    SET_TOOL_TIP_EX(fcgNUAudioBitrate);
    SET_TOOL_TIP_EX(fcgCXAudioPriority);
    SET_TOOL_TIP_EX(fcgCXAudioEncTiming);
    SET_TOOL_TIP_EX(fcgCXAudioTempDir);
    SET_TOOL_TIP_EX(fcgBTCustomAudioTempDir);
    //音声バッチファイル実行
    SET_TOOL_TIP_EX(fcgCBRunBatBeforeAudio);
    SET_TOOL_TIP_EX(fcgCBRunBatAfterAudio);
    SET_TOOL_TIP_EX(fcgBTBatBeforeAudioPath);
    SET_TOOL_TIP_EX(fcgBTBatAfterAudioPath);

    //muxer
    SET_TOOL_TIP_EX(fcgCBMP4MuxerExt);
    SET_TOOL_TIP_EX(fcgCXMP4CmdEx);
    SET_TOOL_TIP_EX(fcgBTMP4MuxerPath);
    SET_TOOL_TIP_EX(fcgBTTC2MP4Path);
    SET_TOOL_TIP_EX(fcgBTMP4RawPath);
    SET_TOOL_TIP_EX(fcgCXMP4BoxTempDir);
    SET_TOOL_TIP_EX(fcgBTMP4BoxTempDir);
    SET_TOOL_TIP_EX(fcgCBMKVMuxerExt);
    SET_TOOL_TIP_EX(fcgCXMKVCmdEx);
    SET_TOOL_TIP_EX(fcgBTMKVMuxerPath);
    SET_TOOL_TIP_EX(fcgCXMuxPriority);
    //バッチファイル実行
    SET_TOOL_TIP_EX(fcgCBRunBatBefore);
    SET_TOOL_TIP_EX(fcgCBRunBatAfter);
    SET_TOOL_TIP_EX(fcgCBWaitForBatBefore);
    SET_TOOL_TIP_EX(fcgCBWaitForBatAfter);
    SET_TOOL_TIP_EX(fcgBTBatBeforePath);
    SET_TOOL_TIP_EX(fcgBTBatAfterPath);
    //他
    SET_TOOL_TIP_EX(fcgTXCmd);
    SET_TOOL_TIP_EX(fcgBTDefault);
    //上部ツールストリップ
    fcgTSBCMDOnly->ToolTipText = LOAD_CLI_STRING(AuofrmTTfcgTSBCMDOnly);
    fcgTSBDelete->ToolTipText = LOAD_CLI_STRING(AuofrmTTfcgTSBDelete);
    fcgTSBOtherSettings->ToolTipText = LOAD_CLI_STRING(AuofrmTTfcgTSBOtherSettings);
    fcgTSBSave->ToolTipText = LOAD_CLI_STRING(AuofrmTTfcgTSBSave);
    fcgTSBSaveNew->ToolTipText = LOAD_CLI_STRING(AuofrmTTfcgTSBSaveNew);
}
System::Void frmConfig::SetX265VersionToolTip(String^ x265Path) {
    String^ mes;
    if (File::Exists(x265Path)) {
        char mes_buf[2560];
        char exe_path[MAX_PATH_LEN];
        GetCHARfromString(exe_path, sizeof(exe_path), x265Path);
        if (get_exe_message(exe_path, "-V", mes_buf, _countof(mes_buf), AUO_PIPE_ENABLE) == RP_SUCCESS)
            mes = String(mes_buf).ToString();
        else
            mes = LOAD_CLI_STRING(AUO_CONFIG_ERR_GET_EXE_VER);
    } else {
        mes = LOAD_CLI_STRING(AUO_CONFIG_ERR_EXE_NOT_FOUND);
    }
    fcgTTX265Version->SetToolTip(fcgTXX265Path, mes);
    fcgTTX265Version->SetToolTip(fcgTXX265PathSub, mes);
}
System::Void frmConfig::ShowExehelp(String^ ExePath, String^ args) {
    if (!File::Exists(ExePath)) {
        MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_EXE_NOT_FOUND), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
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
                MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_GET_HELP), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
                return;
            }
        }
        try {
            System::Diagnostics::Process::Start(String(file_path).ToString());
        } catch (...) {
            MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_OPEN_HELP), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

#pragma warning( pop )
