﻿// -----------------------------------------------------------------------------------------
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

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include "auo.h"
#include "auo_pipe.h"
#include "auo_conf.h"
#include "auo_settings.h"
#include "auo_system.h"
#include "auo_util.h"
#include "auo_clrutil.h"

#include "h264_level.h"
#include "transparentLabel.h"

#include "frmConfig_helper.h"
#include "auo_mes.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;


namespace AUO_NAME_R {

    /// <summary>
    /// frmConfig の概要
    ///
    /// 警告: このクラスの名前を変更する場合、このクラスが依存するすべての .resx ファイルに関連付けられた
    ///          マネージ リソース コンパイラ ツールに対して 'Resource File Name' プロパティを
    ///          変更する必要があります。この変更を行わないと、
    ///          デザイナと、このフォームに関連付けられたローカライズ済みリソースとが、
    ///          正しく相互に利用できなくなります。
    /// </summary>
    public ref class frmConfig : public System::Windows::Forms::Form
    {
    public:
        frmConfig(CONF_GUIEX *_conf, const SYSTEM_DATA *_sys_dat)
        {
            InitData(_conf, _sys_dat);
            list_lng = nullptr;
            dwStgReader = nullptr;
            themeMode = AuoTheme::DefaultLight;
            cnf_fcgTemp = (CONF_ENC*)calloc(1, sizeof(CONF_ENC));
            cnf_stgSelected = (CONF_GUIEX*)calloc(1, sizeof(CONF_GUIEX));
            InitializeComponent();
            //
            //TODO: ここにコンストラクタ コードを追加します
            //
            fcgLastX265ModeAsAMP = true;
            LoadLangText();
        }

    protected:
        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        ~frmConfig()
        {
            if (components)
            {
                delete components;
            }
            CloseBitrateCalc();
            if (nullptr != cnf_fcgTemp) {
                free(cnf_fcgTemp);
            }
            if (nullptr != cnf_stgSelected) {
                free(cnf_stgSelected);
            }
            if (nullptr != qualityTimerX265) {
                delete qualityTimerX265;
            }
            if (nullptr != fcgTabPageVideoController) {
                delete fcgTabPageVideoController;
            }
            if (list_lng != nullptr) {
                delete list_lng;
            }
        }
    private: System::Windows::Forms::Label^  fcgLBAMPAutoBitrate;
    private: System::Windows::Forms::Label^  fchLBAMPAutoBitrate;


    private: System::Windows::Forms::TabControl^  fcgtabControlVideo;



    private: System::Windows::Forms::ToolStrip^  fcgtoolStripSettings;



    private: System::Windows::Forms::TabControl^  fcgtabControlMux;


    private: System::Windows::Forms::TabPage^  fcgtabPageMP4;
    private: System::Windows::Forms::TabPage^  fcgtabPageMKV;
    private: System::Windows::Forms::TextBox^  fcgTXCmd;

    private: System::Windows::Forms::Button^  fcgBTCancel;

    private: System::Windows::Forms::Button^  fcgBTOK;
    private: System::Windows::Forms::Button^  fcgBTDefault;



    private: System::Windows::Forms::TabPage^  fcgtabPageExSettings;
    private: System::Windows::Forms::ToolStripButton^  fcgTSBSave;

    private: System::Windows::Forms::ToolStripButton^  fcgTSBSaveNew;

    private: System::Windows::Forms::ToolStripButton^  fcgTSBDelete;

    private: System::Windows::Forms::ToolStripSeparator^  fcgtoolStripSeparator1;
    private: System::Windows::Forms::ToolStripDropDownButton^  fcgTSSettings;

    private: System::Windows::Forms::ToolStripButton^  fcgTSBCMDOnly;

    private: System::Windows::Forms::GroupBox^  fcggroupBoxExSettings;
    private: System::Windows::Forms::CheckBox^  fcgCBAFSBitrateCorrection;


    private: System::Windows::Forms::CheckBox^  fcgCBAFS;
    private: System::Windows::Forms::Label^  fcgLBX265Priority;

    private: System::Windows::Forms::CheckBox^  fcgCBCheckKeyframes;

    private: System::Windows::Forms::CheckBox^  fcgCBAuoTcfileout;

    private: System::Windows::Forms::Button^  fcgBTCustomTempDir;
    private: System::Windows::Forms::TextBox^  fcgTXCustomTempDir;
    private: System::Windows::Forms::ComboBox^  fcgCXTempDir;
    private: System::Windows::Forms::ComboBox^  fcgCXX264Priority;
    private: System::Windows::Forms::GroupBox^  fcggroupBoxCmdEx;
    private: System::Windows::Forms::Button^  fcgBTCmdEx;
    private: System::Windows::Forms::TextBox^  fcgTXCmdEx;





    private: System::Windows::Forms::TabPage^  fcgtabPageMux;












    private: System::Windows::Forms::ComboBox^  fcgCXMP4CmdEx;

    private: System::Windows::Forms::Label^  fcgLBMP4CmdEx;
    private: System::Windows::Forms::CheckBox^  fcgCBMP4MuxerExt;
    private: System::Windows::Forms::Button^  fcgBTMP4BoxTempDir;
    private: System::Windows::Forms::TextBox^  fcgTXMP4BoxTempDir;


    private: System::Windows::Forms::ComboBox^  fcgCXMP4BoxTempDir;
    private: System::Windows::Forms::Label^  fcgLBMP4BoxTempDir;
    private: System::Windows::Forms::Button^  fcgBTTC2MP4Path;
    private: System::Windows::Forms::TextBox^  fcgTXTC2MP4Path;
    private: System::Windows::Forms::Button^  fcgBTMP4MuxerPath;
    private: System::Windows::Forms::TextBox^  fcgTXMP4MuxerPath;

    private: System::Windows::Forms::Label^  fcgLBTC2MP4Path;
    private: System::Windows::Forms::Label^  fcgLBMP4MuxerPath;


    private: System::Windows::Forms::Button^  fcgBTMKVMuxerPath;

    private: System::Windows::Forms::TextBox^  fcgTXMKVMuxerPath;

    private: System::Windows::Forms::Label^  fcgLBMKVMuxerPath;
    private: System::Windows::Forms::ComboBox^  fcgCXMKVCmdEx;
    private: System::Windows::Forms::Label^  fcgLBMKVMuxerCmdEx;
    private: System::Windows::Forms::CheckBox^  fcgCBMKVMuxerExt;
    private: System::Windows::Forms::ComboBox^  fcgCXMuxPriority;
    private: System::Windows::Forms::Label^  fcgLBMuxPriority;

    private: System::Windows::Forms::Label^  fcgLBVersionDate;


    private: System::Windows::Forms::Label^  fcgLBVersion;
    private: System::Windows::Forms::FolderBrowserDialog^  fcgfolderBrowserTemp;
    private: System::Windows::Forms::OpenFileDialog^  fcgOpenFileDialog;










    private: System::Windows::Forms::ToolTip^  fcgTTEx;


    private: System::Windows::Forms::ToolStripButton^  fcgTSBOtherSettings;
    private: System::Windows::Forms::CheckBox^  fcgCBNulOutCLI;







    private: System::Windows::Forms::Label^  fcgLBTempDir;
    private: System::Windows::Forms::ToolStripButton^  fcgTSBBitrateCalc;
    private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator3;
    private: System::Windows::Forms::ToolStripTextBox^  fcgTSTSettingsNotes;

    private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator4;
    private: System::Windows::Forms::ToolStripLabel^  fcgTSLSettingsNotes;
    private: System::Windows::Forms::CheckBox^  fcgCBMP4MuxApple;


    private: System::Windows::Forms::TabPage^  fcgtabPageBat;
    private: System::Windows::Forms::CheckBox^  fcgCBRunBatBefore;
    private: System::Windows::Forms::Label^  fcgLBBatBeforePath;



    private: System::Windows::Forms::CheckBox^  fcgCBWaitForBatBefore;
    private: System::Windows::Forms::Button^  fcgBTBatBeforePath;


    private: System::Windows::Forms::TextBox^  fcgTXBatBeforePath;




    private: System::Windows::Forms::ContextMenuStrip^  fcgCSReplaceStrings;
























    private: System::Windows::Forms::Button^  fcgBTMP4RawPath;

    private: System::Windows::Forms::TextBox^  fcgTXMP4RawPath;
    private: System::Windows::Forms::Label^  fcgLBMP4RawPath;




    private: System::Windows::Forms::ComboBox^  fcgCXCmdExInsert;
    private: System::Windows::Forms::LinkLabel^  fcgLBguiExBlog;
    private: System::Windows::Forms::Panel^  fcgPNSeparator;
    private: System::Windows::Forms::Button^  fcgBTBatAfterPath;


    private: System::Windows::Forms::TextBox^  fcgTXBatAfterPath;

    private: System::Windows::Forms::Label^  fcgLBBatAfterPath;

    private: System::Windows::Forms::CheckBox^  fcgCBWaitForBatAfter;

    private: System::Windows::Forms::CheckBox^  fcgCBRunBatAfter;
    private: System::Windows::Forms::Label^  fcgLBBatAfterString;

    private: System::Windows::Forms::Label^  fcgLBBatBeforeString;
    private: System::Windows::Forms::CheckBox^  fcgCBInputAsLW48;
    private: System::Windows::Forms::CheckBox^  fcgCBSetKeyframeAtChapter;
private: System::Windows::Forms::TabPage^  fcgtabPageX265Main;
private: System::Windows::Forms::Panel^  fcgPNStatusFile;


private: System::Windows::Forms::Label^  fcgLBSTATUS;
private: System::Windows::Forms::TextBox^  fcgTXStatusFile;
private: System::Windows::Forms::Button^  fcgBTStatusFile;



private: System::Windows::Forms::Panel^  fcgPNBitrate;
private: System::Windows::Forms::Label^  fcgLBQuality;
private: System::Windows::Forms::Label^  fcgLBQualityLeft;
private: System::Windows::Forms::Label^  fcgLBQualityRight;








private: System::Windows::Forms::TextBox^  fcgTXQuality;
private: System::Windows::Forms::Button^  fcgBTTBQualitySubtract;
private: System::Windows::Forms::Button^  fcgBTTBQualityAdd;







private: System::Windows::Forms::TrackBar^  fcgTBQuality;




private: System::Windows::Forms::Panel^  fcgPNX265Mode;
private: System::Windows::Forms::NumericUpDown^  fcgNUAMPLimitBitrate;



private: System::Windows::Forms::NumericUpDown^  fcgNUAMPLimitFileSize;



private: System::Windows::Forms::ComboBox^  fcgCXX265Mode;
private: System::Windows::Forms::CheckBox^  fcgCBNulOut;
private: System::Windows::Forms::CheckBox^  fcgCBAMPLimitBitrate;





private: System::Windows::Forms::CheckBox^  fcgCBFastFirstPass;
private: System::Windows::Forms::NumericUpDown^  fcgNUAutoNPass;
private: System::Windows::Forms::CheckBox^  fcgCBAMPLimitFileSize;






private: System::Windows::Forms::Label^  fcgLBAutoNpass;







private: System::Windows::Forms::PictureBox^  fcgpictureBoxX265;
private: System::Windows::Forms::Button^  fcgBTX265Path;




















private: System::Windows::Forms::TextBox^  fcgTXX265Path;























private: System::Windows::Forms::Label^  fcgLBX265Path;




private: System::Windows::Forms::Panel^  fchPNX265Sub;







private: System::Windows::Forms::TextBox^  fcgTXX265PathSub;




private: System::Windows::Forms::Button^  fcgBTX265PathSub;


private: System::Windows::Forms::ToolTip^  fcgTTX265Version;
private: System::Windows::Forms::Label^  fcgLBX265PathSub;



private: System::Windows::Forms::ToolTip^  fcgTTX265;

private: System::Windows::Forms::GroupBox^  fcggroupBoxPreset;
private: System::Windows::Forms::Button^  fcgBTApplyPreset;




















































private: System::Windows::Forms::ComboBox^  fcgCXProfile;




private: System::Windows::Forms::ComboBox^  fcgCXTune;

private: System::Windows::Forms::ComboBox^  fcgCXPreset;
private: System::Windows::Forms::Label^  fcgLBProfile;







private: System::Windows::Forms::Label^  fcgLBX265Tune;



private: System::Windows::Forms::Label^  fcgLBX265Preset;











private: System::Windows::Forms::TabPage^  fcgtabPageX265Other;










private: System::Windows::Forms::NumericUpDown^  fcgNUCtu;

private: System::Windows::Forms::Label^  fcgLBCtu;
private: System::Windows::Forms::NumericUpDown^  fcgNUTuIntraDepth;
private: System::Windows::Forms::NumericUpDown^  fcgNUTuInterDepth;



private: System::Windows::Forms::Label^  fcgLBTuIntraDepth;
private: System::Windows::Forms::Label^  fcgLBTuInterDepth;
private: System::Windows::Forms::NumericUpDown^  fcgNUMaxMerge;



private: System::Windows::Forms::Label^  fcgLBMaxMerge;
private: System::Windows::Forms::NumericUpDown^  fcgNUMERange;


private: System::Windows::Forms::ComboBox^  fcgCXSubME;

private: System::Windows::Forms::ComboBox^  fcgCXME;

private: System::Windows::Forms::Label^  fcgLBMERange;

private: System::Windows::Forms::Label^  fcgLBSubME;

private: System::Windows::Forms::Label^  fcgLBME;
private: System::Windows::Forms::CheckBox^  fcgCBSAO;


private: System::Windows::Forms::Label^  fcgLBSAO;





private: System::Windows::Forms::NumericUpDown^  fcgNUVBVbuf;







private: System::Windows::Forms::NumericUpDown^  fcgNUVBVmax;

private: System::Windows::Forms::Label^  fcgLBVBVbuf;

private: System::Windows::Forms::Label^  fcgLBVBVmax;
private: System::Windows::Forms::GroupBox^  fcggroupBoxRateControl;


private: System::Windows::Forms::GroupBox^  fcgGroupBoxME;


















private: System::Windows::Forms::GroupBox^  fcgGroupBoxCTU;





private: System::Windows::Forms::ComboBox^  fcgCXCSP;

private: System::Windows::Forms::Label^  fcgLBCSP;
private: System::Windows::Forms::GroupBox^  fcggroupBoxVUIColor;
private: System::Windows::Forms::Label^  fcgLBFullRange;



private: System::Windows::Forms::ComboBox^  fcgCXTransfer;








private: System::Windows::Forms::ComboBox^  fcgCXColorPrim;


private: System::Windows::Forms::ComboBox^  fcgCXColorMatrix;
private: System::Windows::Forms::Label^  fcgLBTransfer;



private: System::Windows::Forms::Label^  fcgLBColorPrim;



private: System::Windows::Forms::Label^  fcgLBColorMatrix;


private: System::Windows::Forms::ComboBox^  fcgCXVideoFormat;


private: System::Windows::Forms::Label^  fcgLBVideoFormat;
private: System::Windows::Forms::GroupBox^  fcgGroupBoxAspectRatio;


private: System::Windows::Forms::NumericUpDown^  fcgNUAspectRatioY;







private: System::Windows::Forms::Label^  fcgLBAspectRatio;

private: System::Windows::Forms::NumericUpDown^  fcgNUAspectRatioX;




private: System::Windows::Forms::ComboBox^  fcgCXAspectRatio;
private: System::Windows::Forms::CheckBox^  fcgCBWeightP;





private: System::Windows::Forms::Label^  fcgLBWeightP;
private: System::Windows::Forms::NumericUpDown^  fcgNUScenecut;


private: System::Windows::Forms::Label^  fcgLBScenecut;
private: System::Windows::Forms::NumericUpDown^  fcgNUKeyintMin;


private: System::Windows::Forms::Label^  fcgLBKeyintMin;
private: System::Windows::Forms::CheckBox^  fcgCBOpenGOP;


private: System::Windows::Forms::Label^  fcgLBOpenGOP;
private: System::Windows::Forms::CheckBox^  fcgCBBpyramid;


private: System::Windows::Forms::Label^  fcgLBBpyramid;
private: System::Windows::Forms::ComboBox^  fcgCXBadapt;


private: System::Windows::Forms::NumericUpDown^  fcgNURCLookahead;

private: System::Windows::Forms::Label^  fcgLBRcLookAhead;
private: System::Windows::Forms::NumericUpDown^  fcgNUKeyintMax;


private: System::Windows::Forms::Label^  fcgLBKeyintMax;
private: System::Windows::Forms::Label^  fcgLBBadapt;
private: System::Windows::Forms::NumericUpDown^  fcgNUBframes;



private: System::Windows::Forms::Label^  fcgLBBframes;
private: System::Windows::Forms::NumericUpDown^  fcgNURef;


private: System::Windows::Forms::Label^  fcgLBRef;
private: System::Windows::Forms::CheckBox^  fcgCBCUTree;


private: System::Windows::Forms::GroupBox^  fcggroupBoxFrame;
private: System::Windows::Forms::Label^  fcgLBCUTree;
private: System::Windows::Forms::NumericUpDown^  fcgNURD;



private: System::Windows::Forms::NumericUpDown^  fcgNUAQStrength;

private: System::Windows::Forms::Label^  fcgLBAQStrength;
private: System::Windows::Forms::ComboBox^  fcgCXAQMode;


private: System::Windows::Forms::Label^  fcgLBAQMode;
private: System::Windows::Forms::Label^  fcgLBRD;


private: System::Windows::Forms::CheckBox^  fcgCBFullRange;



private: System::Windows::Forms::ComboBox^  fcgCXInterlaced;

private: System::Windows::Forms::Label^  fcgLBInterlaced;
private: System::Windows::Forms::Label^  fcgLBWeightB;
private: System::Windows::Forms::CheckBox^  fcgCBWeightB;
private: System::Windows::Forms::NumericUpDown^  fcgNUPsyRD;




private: System::Windows::Forms::Label^  fcgLBPsyRD;


private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator6;
private: System::Windows::Forms::GroupBox^  fcggroupBoxThread;
private: System::Windows::Forms::Label^  fcgLBFrameThreads;
private: System::Windows::Forms::NumericUpDown^  fcgNUFrameThreads;



private: System::Windows::Forms::Label^  fcgLBThreads;
private: System::Windows::Forms::NumericUpDown^  fcgNUPools;
private: System::Windows::Forms::ContextMenuStrip^  fcgCSExeFiles;
private: System::Windows::Forms::ToolStripMenuItem^  fcgTSExeFileshelp;
private: System::Windows::Forms::NumericUpDown^  fcgNUPsyRDOQ;
private: System::Windows::Forms::Label^  fcgLBPsyRDOQ;
private: System::Windows::Forms::Label^  fcgLBPME;

private: System::Windows::Forms::Label^  fcgLBPMode;
private: System::Windows::Forms::CheckBox^  fcgCBPME;

private: System::Windows::Forms::CheckBox^  fcgCBPMode;
private: System::Windows::Forms::Label^  fcgLBWpp;
private: System::Windows::Forms::CheckBox^  fcgCBWpp;
private: System::Windows::Forms::Label^  fcgLBAsymmetricMP;
private: System::Windows::Forms::CheckBox^  fcgCBAsymmetricMP;
private: System::Windows::Forms::Label^  fcgLBRectMP;
private: System::Windows::Forms::CheckBox^  fcgCBRectMP;
private: System::Windows::Forms::Label^  fcgLBQComp;
private: System::Windows::Forms::NumericUpDown^  fcgNUQComp;
private: System::Windows::Forms::CheckBox^  fcgCBDeblock;
private: System::Windows::Forms::GroupBox^  fcggroupBoxDeblock;
private: System::Windows::Forms::NumericUpDown^  fcgNUDeblockThreshold;
private: System::Windows::Forms::NumericUpDown^  fcgNUDeblockStrength;
private: System::Windows::Forms::Label^  fcgLBDeblockThreshold;
private: System::Windows::Forms::Label^  fcgLBDeblockStrength;
private: System::Windows::Forms::NumericUpDown^  fcgNURdoqLevel;
private: System::Windows::Forms::Label^  fcgLBRdoqLevel;
private: System::Windows::Forms::TabControl^  fcgtabControlAudio;
private: System::Windows::Forms::TabPage^  fcgtabPageAudioMain;
private: System::Windows::Forms::ComboBox^  fcgCXAudioDelayCut;
private: System::Windows::Forms::Label^  fcgLBAudioDelayCut;
private: System::Windows::Forms::Label^  fcgCBAudioEncTiming;
private: System::Windows::Forms::ComboBox^  fcgCXAudioEncTiming;
private: System::Windows::Forms::ComboBox^  fcgCXAudioTempDir;
private: System::Windows::Forms::TextBox^  fcgTXCustomAudioTempDir;
private: System::Windows::Forms::Button^  fcgBTCustomAudioTempDir;
private: System::Windows::Forms::CheckBox^  fcgCBAudioUsePipe;
private: System::Windows::Forms::Label^  fcgLBAudioBitrate;
private: System::Windows::Forms::NumericUpDown^  fcgNUAudioBitrate;
private: System::Windows::Forms::CheckBox^  fcgCBAudio2pass;
private: System::Windows::Forms::ComboBox^  fcgCXAudioEncMode;
private: System::Windows::Forms::Label^  fcgLBAudioEncMode;
private: System::Windows::Forms::Button^  fcgBTAudioEncoderPath;
private: System::Windows::Forms::TextBox^  fcgTXAudioEncoderPath;
private: System::Windows::Forms::Label^  fcgLBAudioEncoderPath;
private: System::Windows::Forms::CheckBox^  fcgCBAudioOnly;
private: System::Windows::Forms::CheckBox^  fcgCBFAWCheck;
private: System::Windows::Forms::ComboBox^  fcgCXAudioEncoder;
private: System::Windows::Forms::Label^  fcgLBAudioEncoder;
private: System::Windows::Forms::Label^  fcgLBAudioTemp;
private: System::Windows::Forms::TabPage^  fcgtabPageAudioOther;
private: System::Windows::Forms::Panel^  panel2;
private: System::Windows::Forms::Label^  fcgLBBatAfterAudioString;
private: System::Windows::Forms::Label^  fcgLBBatBeforeAudioString;
private: System::Windows::Forms::Button^  fcgBTBatAfterAudioPath;
private: System::Windows::Forms::TextBox^  fcgTXBatAfterAudioPath;
private: System::Windows::Forms::Label^  fcgLBBatAfterAudioPath;
private: System::Windows::Forms::CheckBox^  fcgCBRunBatAfterAudio;
private: System::Windows::Forms::Panel^  panel1;
private: System::Windows::Forms::Button^  fcgBTBatBeforeAudioPath;
private: System::Windows::Forms::TextBox^  fcgTXBatBeforeAudioPath;
private: System::Windows::Forms::Label^  fcgLBBatBeforeAudioPath;
private: System::Windows::Forms::CheckBox^  fcgCBRunBatBeforeAudio;
private: System::Windows::Forms::ComboBox^  fcgCXAudioPriority;
private: System::Windows::Forms::Label^  fcgLBAudioPriority;
private: System::Windows::Forms::Label^  fcgLBQGSize;
private: System::Windows::Forms::NumericUpDown^  fcgNUQGSize;
private: System::Windows::Forms::ComboBox^  fcgCXBitDepth;
private: System::Windows::Forms::Label^  fcgLBBitDepth;
private: System::Windows::Forms::Label^  fcgLBNoRecusrionSkip;


private: System::Windows::Forms::Label^  fcgLBLimitModes;
private: System::Windows::Forms::CheckBox^  fcgCBLimitModes;
private: System::Windows::Forms::Label^  fcgLBLimitRefs;
private: System::Windows::Forms::NumericUpDown^  fcgNULimitRefs;
private: System::Windows::Forms::Label^  fcgLBAQMotion;
private: System::Windows::Forms::CheckBox^  fcgCBAQMotion;


private: System::Windows::Forms::CheckBox^  fcgCBSsimRd;
private: System::Windows::Forms::Label^  fcgLBSsimRd;

private: System::Windows::Forms::TabPage^  fcgtabPageX2652pass;
private: System::Windows::Forms::CheckBox^  fcgCBAnalysisReuse;
private: System::Windows::Forms::GroupBox^  fcggroupBoxAnalysisReuse;
private: System::Windows::Forms::NumericUpDown^  fcgNURefineInter;

private: System::Windows::Forms::NumericUpDown^  fcgNURefineIntra;

private: System::Windows::Forms::Label^  fcgTXAnalysisRefineInter;
private: System::Windows::Forms::Label^  fcgTXAnalysisRefineIntra;
private: System::Windows::Forms::Label^  fcgLBAnalysisReuseLevel;
private: System::Windows::Forms::Label^  fcgLBAnalysisReuseFile;
private: System::Windows::Forms::TextBox^  fcgTXAnalysisReuseFile;
private: System::Windows::Forms::Button^  fcgBTAnalysisReuseFile;
private: System::Windows::Forms::NumericUpDown^  fcgNUAnalysisReuseLevel;
private: System::Windows::Forms::CheckBox^  fcgCBHEVCAQ;
private: System::Windows::Forms::Label^  fcgLBHEVCAQ;
private: System::Windows::Forms::CheckBox ^fcgCBSVT;
private: System::Windows::Forms::CheckBox ^fcgCBSyncProcessAffinity;
private: System::Windows::Forms::NumericUpDown^ fcgNURskip;
private: System::Windows::Forms::Label ^fcgLBFade;
private: System::Windows::Forms::CheckBox ^fcgCBFade;
private: System::Windows::Forms::Label ^fcgLBSceneCutMode;
private: System::Windows::Forms::ComboBox ^fcgCXSceneCutMode;
private: System::Windows::Forms::NumericUpDown ^fcgNUHistThreshold;
private: System::Windows::Forms::NumericUpDown ^fcgNURskipEdgeThreshold;
private: System::Windows::Forms::Label ^fcgLBRskipEdgeThreshold;
private: System::Windows::Forms::Panel^  fcgPNHideTabControlMux;
private: System::Windows::Forms::Panel^  fcgPNHideTabControlAudio;
private: System::Windows::Forms::Panel^  fcgPNHideTabControlVideo;
private: System::Windows::Forms::Panel^  fcgPNHideToolStripBorder;
private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
private: System::Windows::Forms::ToolStripDropDownButton^  fcgTSLanguage;









































































    private: System::ComponentModel::IContainer^  components;




    private:
        /// <summary>
        /// 必要なデザイナ変数です。
        /// </summary>


#pragma region Windows Form Designer generated code
        /// <summary>
        /// デザイナ サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディタで変更しないでください。
        /// </summary>
        void InitializeComponent(void)
        {
            this->components = (gcnew System::ComponentModel::Container());
            System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(frmConfig::typeid));
            this->fcgtabControlVideo = (gcnew System::Windows::Forms::TabControl());
            this->fcgtabPageX265Main = (gcnew System::Windows::Forms::TabPage());
            this->fcgCXBitDepth = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBBitDepth = (gcnew System::Windows::Forms::Label());
            this->fcggroupBoxThread = (gcnew System::Windows::Forms::GroupBox());
            this->fcgLBPME = (gcnew System::Windows::Forms::Label());
            this->fcgLBPMode = (gcnew System::Windows::Forms::Label());
            this->fcgCBPME = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBPMode = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBWpp = (gcnew System::Windows::Forms::Label());
            this->fcgCBWpp = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBFrameThreads = (gcnew System::Windows::Forms::Label());
            this->fcgNUFrameThreads = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBThreads = (gcnew System::Windows::Forms::Label());
            this->fcgNUPools = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgCXInterlaced = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBInterlaced = (gcnew System::Windows::Forms::Label());
            this->fcggroupBoxVUIColor = (gcnew System::Windows::Forms::GroupBox());
            this->fcgCBFullRange = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBFullRange = (gcnew System::Windows::Forms::Label());
            this->fcgCXTransfer = (gcnew System::Windows::Forms::ComboBox());
            this->fcgCXColorPrim = (gcnew System::Windows::Forms::ComboBox());
            this->fcgCXColorMatrix = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBTransfer = (gcnew System::Windows::Forms::Label());
            this->fcgLBColorPrim = (gcnew System::Windows::Forms::Label());
            this->fcgLBColorMatrix = (gcnew System::Windows::Forms::Label());
            this->fcgCXVideoFormat = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBVideoFormat = (gcnew System::Windows::Forms::Label());
            this->fcgGroupBoxAspectRatio = (gcnew System::Windows::Forms::GroupBox());
            this->fcgNUAspectRatioY = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBAspectRatio = (gcnew System::Windows::Forms::Label());
            this->fcgNUAspectRatioX = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgCXAspectRatio = (gcnew System::Windows::Forms::ComboBox());
            this->fcgCXCSP = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBCSP = (gcnew System::Windows::Forms::Label());
            this->fcggroupBoxRateControl = (gcnew System::Windows::Forms::GroupBox());
            this->fcgLBQComp = (gcnew System::Windows::Forms::Label());
            this->fcgNUQComp = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBVBVmax = (gcnew System::Windows::Forms::Label());
            this->fcgNUVBVbuf = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBVBVbuf = (gcnew System::Windows::Forms::Label());
            this->fcgNUVBVmax = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcggroupBoxPreset = (gcnew System::Windows::Forms::GroupBox());
            this->fcgBTApplyPreset = (gcnew System::Windows::Forms::Button());
            this->fcgCXProfile = (gcnew System::Windows::Forms::ComboBox());
            this->fcgCXTune = (gcnew System::Windows::Forms::ComboBox());
            this->fcgCXPreset = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBProfile = (gcnew System::Windows::Forms::Label());
            this->fcgLBX265Tune = (gcnew System::Windows::Forms::Label());
            this->fcgLBX265Preset = (gcnew System::Windows::Forms::Label());
            this->fcgPNStatusFile = (gcnew System::Windows::Forms::Panel());
            this->fcgLBSTATUS = (gcnew System::Windows::Forms::Label());
            this->fcgTXStatusFile = (gcnew System::Windows::Forms::TextBox());
            this->fcgBTStatusFile = (gcnew System::Windows::Forms::Button());
            this->fcgPNBitrate = (gcnew System::Windows::Forms::Panel());
            this->fcgLBQuality = (gcnew System::Windows::Forms::Label());
            this->fcgLBQualityLeft = (gcnew System::Windows::Forms::Label());
            this->fcgLBQualityRight = (gcnew System::Windows::Forms::Label());
            this->fcgTXQuality = (gcnew System::Windows::Forms::TextBox());
            this->fcgBTTBQualitySubtract = (gcnew System::Windows::Forms::Button());
            this->fcgBTTBQualityAdd = (gcnew System::Windows::Forms::Button());
            this->fcgTBQuality = (gcnew System::Windows::Forms::TrackBar());
            this->fcgPNX265Mode = (gcnew System::Windows::Forms::Panel());
            this->fcgNUAMPLimitBitrate = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgNUAMPLimitFileSize = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgCXX265Mode = (gcnew System::Windows::Forms::ComboBox());
            this->fcgCBNulOut = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBAMPLimitBitrate = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBFastFirstPass = (gcnew System::Windows::Forms::CheckBox());
            this->fcgNUAutoNPass = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgCBAMPLimitFileSize = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBAutoNpass = (gcnew System::Windows::Forms::Label());
            this->fcgpictureBoxX265 = (gcnew System::Windows::Forms::PictureBox());
            this->fcgBTX265Path = (gcnew System::Windows::Forms::Button());
            this->fcgTXX265Path = (gcnew System::Windows::Forms::TextBox());
            this->fcgLBX265Path = (gcnew System::Windows::Forms::Label());
            this->fcgtabPageX265Other = (gcnew System::Windows::Forms::TabPage());
            this->fcgCBHEVCAQ = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBHEVCAQ = (gcnew System::Windows::Forms::Label());
            this->fcgCBSsimRd = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBSsimRd = (gcnew System::Windows::Forms::Label());
            this->fcgLBAQMotion = (gcnew System::Windows::Forms::Label());
            this->fcgCBAQMotion = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBQGSize = (gcnew System::Windows::Forms::Label());
            this->fcgNUQGSize = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgNURdoqLevel = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBRdoqLevel = (gcnew System::Windows::Forms::Label());
            this->fcgCBDeblock = (gcnew System::Windows::Forms::CheckBox());
            this->fcggroupBoxDeblock = (gcnew System::Windows::Forms::GroupBox());
            this->fcgNUDeblockThreshold = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgNUDeblockStrength = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBDeblockThreshold = (gcnew System::Windows::Forms::Label());
            this->fcgLBDeblockStrength = (gcnew System::Windows::Forms::Label());
            this->fcgNUPsyRDOQ = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBPsyRDOQ = (gcnew System::Windows::Forms::Label());
            this->fcgNUPsyRD = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBPsyRD = (gcnew System::Windows::Forms::Label());
            this->fcggroupBoxFrame = (gcnew System::Windows::Forms::GroupBox());
            this->fcgNUHistThreshold = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBFade = (gcnew System::Windows::Forms::Label());
            this->fcgCBFade = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBSceneCutMode = (gcnew System::Windows::Forms::Label());
            this->fcgCXSceneCutMode = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBWeightB = (gcnew System::Windows::Forms::Label());
            this->fcgCBWeightB = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBScenecut = (gcnew System::Windows::Forms::Label());
            this->fcgLBRef = (gcnew System::Windows::Forms::Label());
            this->fcgNURef = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBBframes = (gcnew System::Windows::Forms::Label());
            this->fcgNUBframes = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBBadapt = (gcnew System::Windows::Forms::Label());
            this->fcgLBKeyintMax = (gcnew System::Windows::Forms::Label());
            this->fcgNUKeyintMax = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBRcLookAhead = (gcnew System::Windows::Forms::Label());
            this->fcgCBWeightP = (gcnew System::Windows::Forms::CheckBox());
            this->fcgNURCLookahead = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBWeightP = (gcnew System::Windows::Forms::Label());
            this->fcgCXBadapt = (gcnew System::Windows::Forms::ComboBox());
            this->fcgNUScenecut = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBBpyramid = (gcnew System::Windows::Forms::Label());
            this->fcgCBBpyramid = (gcnew System::Windows::Forms::CheckBox());
            this->fcgNUKeyintMin = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBOpenGOP = (gcnew System::Windows::Forms::Label());
            this->fcgLBKeyintMin = (gcnew System::Windows::Forms::Label());
            this->fcgCBOpenGOP = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBCUTree = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBCUTree = (gcnew System::Windows::Forms::Label());
            this->fcgNURD = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgNUAQStrength = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBAQStrength = (gcnew System::Windows::Forms::Label());
            this->fcgCXAQMode = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBAQMode = (gcnew System::Windows::Forms::Label());
            this->fcgLBRD = (gcnew System::Windows::Forms::Label());
            this->fcgGroupBoxME = (gcnew System::Windows::Forms::GroupBox());
            this->fcgNURskipEdgeThreshold = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBRskipEdgeThreshold = (gcnew System::Windows::Forms::Label());
            this->fcgNURskip = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBNoRecusrionSkip = (gcnew System::Windows::Forms::Label());
            this->fcgLBME = (gcnew System::Windows::Forms::Label());
            this->fcgLBSubME = (gcnew System::Windows::Forms::Label());
            this->fcgNUMaxMerge = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBMERange = (gcnew System::Windows::Forms::Label());
            this->fcgLBMaxMerge = (gcnew System::Windows::Forms::Label());
            this->fcgCXME = (gcnew System::Windows::Forms::ComboBox());
            this->fcgNUMERange = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgCXSubME = (gcnew System::Windows::Forms::ComboBox());
            this->fcgGroupBoxCTU = (gcnew System::Windows::Forms::GroupBox());
            this->fcgLBLimitModes = (gcnew System::Windows::Forms::Label());
            this->fcgCBLimitModes = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBLimitRefs = (gcnew System::Windows::Forms::Label());
            this->fcgNULimitRefs = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBAsymmetricMP = (gcnew System::Windows::Forms::Label());
            this->fcgCBAsymmetricMP = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBRectMP = (gcnew System::Windows::Forms::Label());
            this->fcgCBRectMP = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBCtu = (gcnew System::Windows::Forms::Label());
            this->fcgLBTuInterDepth = (gcnew System::Windows::Forms::Label());
            this->fcgNUCtu = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgLBTuIntraDepth = (gcnew System::Windows::Forms::Label());
            this->fcgNUTuInterDepth = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgNUTuIntraDepth = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgCBSAO = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBSAO = (gcnew System::Windows::Forms::Label());
            this->fcgtabPageX2652pass = (gcnew System::Windows::Forms::TabPage());
            this->fcgCBSVT = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBAnalysisReuse = (gcnew System::Windows::Forms::CheckBox());
            this->fcggroupBoxAnalysisReuse = (gcnew System::Windows::Forms::GroupBox());
            this->fcgNURefineInter = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgNURefineIntra = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgTXAnalysisRefineInter = (gcnew System::Windows::Forms::Label());
            this->fcgTXAnalysisRefineIntra = (gcnew System::Windows::Forms::Label());
            this->fcgLBAnalysisReuseLevel = (gcnew System::Windows::Forms::Label());
            this->fcgLBAnalysisReuseFile = (gcnew System::Windows::Forms::Label());
            this->fcgTXAnalysisReuseFile = (gcnew System::Windows::Forms::TextBox());
            this->fcgBTAnalysisReuseFile = (gcnew System::Windows::Forms::Button());
            this->fcgNUAnalysisReuseLevel = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgtabPageExSettings = (gcnew System::Windows::Forms::TabPage());
            this->fcgCBSyncProcessAffinity = (gcnew System::Windows::Forms::CheckBox());
            this->fchPNX265Sub = (gcnew System::Windows::Forms::Panel());
            this->fcgLBX265PathSub = (gcnew System::Windows::Forms::Label());
            this->fcgTXX265PathSub = (gcnew System::Windows::Forms::TextBox());
            this->fcgBTX265PathSub = (gcnew System::Windows::Forms::Button());
            this->fcgLBTempDir = (gcnew System::Windows::Forms::Label());
            this->fcgBTCustomTempDir = (gcnew System::Windows::Forms::Button());
            this->fcggroupBoxCmdEx = (gcnew System::Windows::Forms::GroupBox());
            this->fcgCXCmdExInsert = (gcnew System::Windows::Forms::ComboBox());
            this->fcgCBNulOutCLI = (gcnew System::Windows::Forms::CheckBox());
            this->fcgBTCmdEx = (gcnew System::Windows::Forms::Button());
            this->fcgTXCmdEx = (gcnew System::Windows::Forms::TextBox());
            this->fcgTXCustomTempDir = (gcnew System::Windows::Forms::TextBox());
            this->fcgCXTempDir = (gcnew System::Windows::Forms::ComboBox());
            this->fcgCXX264Priority = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBX265Priority = (gcnew System::Windows::Forms::Label());
            this->fcggroupBoxExSettings = (gcnew System::Windows::Forms::GroupBox());
            this->fcgCBSetKeyframeAtChapter = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBInputAsLW48 = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBCheckKeyframes = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBAuoTcfileout = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBAFSBitrateCorrection = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBAFS = (gcnew System::Windows::Forms::CheckBox());
            this->fcgtoolStripSettings = (gcnew System::Windows::Forms::ToolStrip());
            this->fcgTSBSave = (gcnew System::Windows::Forms::ToolStripButton());
            this->fcgTSBSaveNew = (gcnew System::Windows::Forms::ToolStripButton());
            this->fcgTSBDelete = (gcnew System::Windows::Forms::ToolStripButton());
            this->fcgtoolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->fcgTSSettings = (gcnew System::Windows::Forms::ToolStripDropDownButton());
            this->fcgTSBCMDOnly = (gcnew System::Windows::Forms::ToolStripButton());
            this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->fcgTSLanguage = (gcnew System::Windows::Forms::ToolStripDropDownButton());
            this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->fcgTSBBitrateCalc = (gcnew System::Windows::Forms::ToolStripButton());
            this->toolStripSeparator6 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->fcgTSBOtherSettings = (gcnew System::Windows::Forms::ToolStripButton());
            this->fcgTSLSettingsNotes = (gcnew System::Windows::Forms::ToolStripLabel());
            this->fcgTSTSettingsNotes = (gcnew System::Windows::Forms::ToolStripTextBox());
            this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
            this->fcgtabControlMux = (gcnew System::Windows::Forms::TabControl());
            this->fcgtabPageMP4 = (gcnew System::Windows::Forms::TabPage());
            this->fcgCBMP4MuxApple = (gcnew System::Windows::Forms::CheckBox());
            this->fcgBTTC2MP4Path = (gcnew System::Windows::Forms::Button());
            this->fcgTXTC2MP4Path = (gcnew System::Windows::Forms::TextBox());
            this->fcgBTMP4MuxerPath = (gcnew System::Windows::Forms::Button());
            this->fcgTXMP4MuxerPath = (gcnew System::Windows::Forms::TextBox());
            this->fcgLBTC2MP4Path = (gcnew System::Windows::Forms::Label());
            this->fcgLBMP4MuxerPath = (gcnew System::Windows::Forms::Label());
            this->fcgCXMP4CmdEx = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBMP4CmdEx = (gcnew System::Windows::Forms::Label());
            this->fcgCBMP4MuxerExt = (gcnew System::Windows::Forms::CheckBox());
            this->fcgBTMP4RawPath = (gcnew System::Windows::Forms::Button());
            this->fcgTXMP4RawPath = (gcnew System::Windows::Forms::TextBox());
            this->fcgLBMP4RawPath = (gcnew System::Windows::Forms::Label());
            this->fcgBTMP4BoxTempDir = (gcnew System::Windows::Forms::Button());
            this->fcgTXMP4BoxTempDir = (gcnew System::Windows::Forms::TextBox());
            this->fcgCXMP4BoxTempDir = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBMP4BoxTempDir = (gcnew System::Windows::Forms::Label());
            this->fcgtabPageMKV = (gcnew System::Windows::Forms::TabPage());
            this->fcgBTMKVMuxerPath = (gcnew System::Windows::Forms::Button());
            this->fcgTXMKVMuxerPath = (gcnew System::Windows::Forms::TextBox());
            this->fcgLBMKVMuxerPath = (gcnew System::Windows::Forms::Label());
            this->fcgCXMKVCmdEx = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBMKVMuxerCmdEx = (gcnew System::Windows::Forms::Label());
            this->fcgCBMKVMuxerExt = (gcnew System::Windows::Forms::CheckBox());
            this->fcgtabPageMux = (gcnew System::Windows::Forms::TabPage());
            this->fcgCXMuxPriority = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBMuxPriority = (gcnew System::Windows::Forms::Label());
            this->fcgtabPageBat = (gcnew System::Windows::Forms::TabPage());
            this->fcgLBBatAfterString = (gcnew System::Windows::Forms::Label());
            this->fcgLBBatBeforeString = (gcnew System::Windows::Forms::Label());
            this->fcgBTBatAfterPath = (gcnew System::Windows::Forms::Button());
            this->fcgTXBatAfterPath = (gcnew System::Windows::Forms::TextBox());
            this->fcgLBBatAfterPath = (gcnew System::Windows::Forms::Label());
            this->fcgCBWaitForBatAfter = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBRunBatAfter = (gcnew System::Windows::Forms::CheckBox());
            this->fcgPNSeparator = (gcnew System::Windows::Forms::Panel());
            this->fcgBTBatBeforePath = (gcnew System::Windows::Forms::Button());
            this->fcgTXBatBeforePath = (gcnew System::Windows::Forms::TextBox());
            this->fcgLBBatBeforePath = (gcnew System::Windows::Forms::Label());
            this->fcgCBWaitForBatBefore = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBRunBatBefore = (gcnew System::Windows::Forms::CheckBox());
            this->fcgTXCmd = (gcnew System::Windows::Forms::TextBox());
            this->fcgBTCancel = (gcnew System::Windows::Forms::Button());
            this->fcgBTOK = (gcnew System::Windows::Forms::Button());
            this->fcgBTDefault = (gcnew System::Windows::Forms::Button());
            this->fcgLBVersionDate = (gcnew System::Windows::Forms::Label());
            this->fcgLBVersion = (gcnew System::Windows::Forms::Label());
            this->fcgfolderBrowserTemp = (gcnew System::Windows::Forms::FolderBrowserDialog());
            this->fcgOpenFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
            this->fcgTTEx = (gcnew System::Windows::Forms::ToolTip(this->components));
            this->fcgCSReplaceStrings = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
            this->fcgLBguiExBlog = (gcnew System::Windows::Forms::LinkLabel());
            this->fcgTTX265Version = (gcnew System::Windows::Forms::ToolTip(this->components));
            this->fcgTTX265 = (gcnew System::Windows::Forms::ToolTip(this->components));
            this->fcgCSExeFiles = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
            this->fcgTSExeFileshelp = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->fcgtabControlAudio = (gcnew System::Windows::Forms::TabControl());
            this->fcgtabPageAudioMain = (gcnew System::Windows::Forms::TabPage());
            this->fcgCXAudioDelayCut = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBAudioDelayCut = (gcnew System::Windows::Forms::Label());
            this->fcgCBAudioEncTiming = (gcnew System::Windows::Forms::Label());
            this->fcgCXAudioEncTiming = (gcnew System::Windows::Forms::ComboBox());
            this->fcgCXAudioTempDir = (gcnew System::Windows::Forms::ComboBox());
            this->fcgTXCustomAudioTempDir = (gcnew System::Windows::Forms::TextBox());
            this->fcgBTCustomAudioTempDir = (gcnew System::Windows::Forms::Button());
            this->fcgCBAudioUsePipe = (gcnew System::Windows::Forms::CheckBox());
            this->fcgLBAudioBitrate = (gcnew System::Windows::Forms::Label());
            this->fcgNUAudioBitrate = (gcnew System::Windows::Forms::NumericUpDown());
            this->fcgCBAudio2pass = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCXAudioEncMode = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBAudioEncMode = (gcnew System::Windows::Forms::Label());
            this->fcgBTAudioEncoderPath = (gcnew System::Windows::Forms::Button());
            this->fcgTXAudioEncoderPath = (gcnew System::Windows::Forms::TextBox());
            this->fcgLBAudioEncoderPath = (gcnew System::Windows::Forms::Label());
            this->fcgCBAudioOnly = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCBFAWCheck = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCXAudioEncoder = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBAudioEncoder = (gcnew System::Windows::Forms::Label());
            this->fcgLBAudioTemp = (gcnew System::Windows::Forms::Label());
            this->fcgtabPageAudioOther = (gcnew System::Windows::Forms::TabPage());
            this->panel2 = (gcnew System::Windows::Forms::Panel());
            this->fcgLBBatAfterAudioString = (gcnew System::Windows::Forms::Label());
            this->fcgLBBatBeforeAudioString = (gcnew System::Windows::Forms::Label());
            this->fcgBTBatAfterAudioPath = (gcnew System::Windows::Forms::Button());
            this->fcgTXBatAfterAudioPath = (gcnew System::Windows::Forms::TextBox());
            this->fcgLBBatAfterAudioPath = (gcnew System::Windows::Forms::Label());
            this->fcgCBRunBatAfterAudio = (gcnew System::Windows::Forms::CheckBox());
            this->panel1 = (gcnew System::Windows::Forms::Panel());
            this->fcgBTBatBeforeAudioPath = (gcnew System::Windows::Forms::Button());
            this->fcgTXBatBeforeAudioPath = (gcnew System::Windows::Forms::TextBox());
            this->fcgLBBatBeforeAudioPath = (gcnew System::Windows::Forms::Label());
            this->fcgCBRunBatBeforeAudio = (gcnew System::Windows::Forms::CheckBox());
            this->fcgCXAudioPriority = (gcnew System::Windows::Forms::ComboBox());
            this->fcgLBAudioPriority = (gcnew System::Windows::Forms::Label());
            this->fcgPNHideTabControlMux = (gcnew System::Windows::Forms::Panel());
            this->fcgPNHideTabControlAudio = (gcnew System::Windows::Forms::Panel());
            this->fcgPNHideTabControlVideo = (gcnew System::Windows::Forms::Panel());
            this->fcgPNHideToolStripBorder = (gcnew System::Windows::Forms::Panel());
            this->fcgtabControlVideo->SuspendLayout();
            this->fcgtabPageX265Main->SuspendLayout();
            this->fcggroupBoxThread->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUFrameThreads))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUPools))->BeginInit();
            this->fcggroupBoxVUIColor->SuspendLayout();
            this->fcgGroupBoxAspectRatio->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAspectRatioY))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAspectRatioX))->BeginInit();
            this->fcggroupBoxRateControl->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUQComp))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUVBVbuf))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUVBVmax))->BeginInit();
            this->fcggroupBoxPreset->SuspendLayout();
            this->fcgPNStatusFile->SuspendLayout();
            this->fcgPNBitrate->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgTBQuality))->BeginInit();
            this->fcgPNX265Mode->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAMPLimitBitrate))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAMPLimitFileSize))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAutoNPass))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgpictureBoxX265))->BeginInit();
            this->fcgtabPageX265Other->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUQGSize))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURdoqLevel))->BeginInit();
            this->fcggroupBoxDeblock->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUDeblockThreshold))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUDeblockStrength))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUPsyRDOQ))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUPsyRD))->BeginInit();
            this->fcggroupBoxFrame->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUHistThreshold))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURef))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUBframes))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUKeyintMax))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURCLookahead))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUScenecut))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUKeyintMin))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURD))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAQStrength))->BeginInit();
            this->fcgGroupBoxME->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURskipEdgeThreshold))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURskip))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUMaxMerge))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUMERange))->BeginInit();
            this->fcgGroupBoxCTU->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNULimitRefs))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUCtu))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUTuInterDepth))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUTuIntraDepth))->BeginInit();
            this->fcgtabPageX2652pass->SuspendLayout();
            this->fcggroupBoxAnalysisReuse->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURefineInter))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURefineIntra))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAnalysisReuseLevel))->BeginInit();
            this->fcgtabPageExSettings->SuspendLayout();
            this->fchPNX265Sub->SuspendLayout();
            this->fcggroupBoxCmdEx->SuspendLayout();
            this->fcggroupBoxExSettings->SuspendLayout();
            this->fcgtoolStripSettings->SuspendLayout();
            this->fcgtabControlMux->SuspendLayout();
            this->fcgtabPageMP4->SuspendLayout();
            this->fcgtabPageMKV->SuspendLayout();
            this->fcgtabPageMux->SuspendLayout();
            this->fcgtabPageBat->SuspendLayout();
            this->fcgCSExeFiles->SuspendLayout();
            this->fcgtabControlAudio->SuspendLayout();
            this->fcgtabPageAudioMain->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAudioBitrate))->BeginInit();
            this->fcgtabPageAudioOther->SuspendLayout();
            this->fcgPNHideTabControlMux->SuspendLayout();
            this->fcgPNHideTabControlAudio->SuspendLayout();
            this->fcgPNHideTabControlVideo->SuspendLayout();
            this->SuspendLayout();
            // 
            // fcgtabControlVideo
            // 
            this->fcgtabControlVideo->Controls->Add(this->fcgtabPageX265Main);
            this->fcgtabControlVideo->Controls->Add(this->fcgtabPageX265Other);
            this->fcgtabControlVideo->Controls->Add(this->fcgtabPageX2652pass);
            this->fcgtabControlVideo->Controls->Add(this->fcgtabPageExSettings);
            this->fcgtabControlVideo->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->fcgtabControlVideo->Location = System::Drawing::Point(2, 2);
            this->fcgtabControlVideo->Name = L"fcgtabControlVideo";
            this->fcgtabControlVideo->SelectedIndex = 0;
            this->fcgtabControlVideo->Size = System::Drawing::Size(616, 520);
            this->fcgtabControlVideo->TabIndex = 0;
            // 
            // fcgtabPageX265Main
            // 
            this->fcgtabPageX265Main->Controls->Add(this->fcgCXBitDepth);
            this->fcgtabPageX265Main->Controls->Add(this->fcgLBBitDepth);
            this->fcgtabPageX265Main->Controls->Add(this->fcggroupBoxThread);
            this->fcgtabPageX265Main->Controls->Add(this->fcgCXInterlaced);
            this->fcgtabPageX265Main->Controls->Add(this->fcgLBInterlaced);
            this->fcgtabPageX265Main->Controls->Add(this->fcggroupBoxVUIColor);
            this->fcgtabPageX265Main->Controls->Add(this->fcgCXVideoFormat);
            this->fcgtabPageX265Main->Controls->Add(this->fcgLBVideoFormat);
            this->fcgtabPageX265Main->Controls->Add(this->fcgGroupBoxAspectRatio);
            this->fcgtabPageX265Main->Controls->Add(this->fcgCXCSP);
            this->fcgtabPageX265Main->Controls->Add(this->fcgLBCSP);
            this->fcgtabPageX265Main->Controls->Add(this->fcggroupBoxRateControl);
            this->fcgtabPageX265Main->Controls->Add(this->fcggroupBoxPreset);
            this->fcgtabPageX265Main->Controls->Add(this->fcgPNStatusFile);
            this->fcgtabPageX265Main->Controls->Add(this->fcgPNBitrate);
            this->fcgtabPageX265Main->Controls->Add(this->fcgPNX265Mode);
            this->fcgtabPageX265Main->Controls->Add(this->fcgpictureBoxX265);
            this->fcgtabPageX265Main->Controls->Add(this->fcgBTX265Path);
            this->fcgtabPageX265Main->Controls->Add(this->fcgTXX265Path);
            this->fcgtabPageX265Main->Controls->Add(this->fcgLBX265Path);
            this->fcgtabPageX265Main->Location = System::Drawing::Point(4, 23);
            this->fcgtabPageX265Main->Name = L"fcgtabPageX265Main";
            this->fcgtabPageX265Main->Padding = System::Windows::Forms::Padding(3);
            this->fcgtabPageX265Main->Size = System::Drawing::Size(608, 493);
            this->fcgtabPageX265Main->TabIndex = 5;
            this->fcgtabPageX265Main->Text = L" x265 (1)";
            this->fcgtabPageX265Main->UseVisualStyleBackColor = true;
            // 
            // fcgCXBitDepth
            // 
            this->fcgCXBitDepth->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXBitDepth->FormattingEnabled = true;
            this->fcgCXBitDepth->Location = System::Drawing::Point(494, 284);
            this->fcgCXBitDepth->Name = L"fcgCXBitDepth";
            this->fcgCXBitDepth->Size = System::Drawing::Size(95, 22);
            this->fcgCXBitDepth->TabIndex = 65;
            this->fcgCXBitDepth->Tag = L"reCmd";
            // 
            // fcgLBBitDepth
            // 
            this->fcgLBBitDepth->AutoSize = true;
            this->fcgLBBitDepth->Location = System::Drawing::Point(396, 287);
            this->fcgLBBitDepth->Name = L"fcgLBBitDepth";
            this->fcgLBBitDepth->Size = System::Drawing::Size(62, 14);
            this->fcgLBBitDepth->TabIndex = 73;
            this->fcgLBBitDepth->Text = L"出力色深度";
            // 
            // fcggroupBoxThread
            // 
            this->fcggroupBoxThread->Controls->Add(this->fcgLBPME);
            this->fcggroupBoxThread->Controls->Add(this->fcgLBPMode);
            this->fcggroupBoxThread->Controls->Add(this->fcgCBPME);
            this->fcggroupBoxThread->Controls->Add(this->fcgCBPMode);
            this->fcggroupBoxThread->Controls->Add(this->fcgLBWpp);
            this->fcggroupBoxThread->Controls->Add(this->fcgCBWpp);
            this->fcggroupBoxThread->Controls->Add(this->fcgLBFrameThreads);
            this->fcggroupBoxThread->Controls->Add(this->fcgNUFrameThreads);
            this->fcggroupBoxThread->Controls->Add(this->fcgLBThreads);
            this->fcggroupBoxThread->Controls->Add(this->fcgNUPools);
            this->fcggroupBoxThread->Location = System::Drawing::Point(196, 305);
            this->fcggroupBoxThread->Name = L"fcggroupBoxThread";
            this->fcggroupBoxThread->Size = System::Drawing::Size(177, 182);
            this->fcggroupBoxThread->TabIndex = 40;
            this->fcggroupBoxThread->TabStop = false;
            // 
            // fcgLBPME
            // 
            this->fcgLBPME->AutoSize = true;
            this->fcgLBPME->Location = System::Drawing::Point(10, 144);
            this->fcgLBPME->Name = L"fcgLBPME";
            this->fcgLBPME->Size = System::Drawing::Size(70, 14);
            this->fcgLBPME->TabIndex = 77;
            this->fcgLBPME->Text = L"並列動き予測";
            // 
            // fcgLBPMode
            // 
            this->fcgLBPMode->AutoSize = true;
            this->fcgLBPMode->Location = System::Drawing::Point(10, 114);
            this->fcgLBPMode->Name = L"fcgLBPMode";
            this->fcgLBPMode->Size = System::Drawing::Size(76, 14);
            this->fcgLBPMode->TabIndex = 76;
            this->fcgLBPMode->Text = L"並列モード解析";
            // 
            // fcgCBPME
            // 
            this->fcgCBPME->AutoSize = true;
            this->fcgCBPME->Location = System::Drawing::Point(145, 145);
            this->fcgCBPME->Name = L"fcgCBPME";
            this->fcgCBPME->Size = System::Drawing::Size(15, 14);
            this->fcgCBPME->TabIndex = 45;
            this->fcgCBPME->Tag = L"reCmd";
            this->fcgCBPME->UseVisualStyleBackColor = true;
            // 
            // fcgCBPMode
            // 
            this->fcgCBPMode->AutoSize = true;
            this->fcgCBPMode->Location = System::Drawing::Point(145, 116);
            this->fcgCBPMode->Name = L"fcgCBPMode";
            this->fcgCBPMode->Size = System::Drawing::Size(15, 14);
            this->fcgCBPMode->TabIndex = 44;
            this->fcgCBPMode->Tag = L"reCmd";
            this->fcgCBPMode->UseVisualStyleBackColor = true;
            // 
            // fcgLBWpp
            // 
            this->fcgLBWpp->AutoSize = true;
            this->fcgLBWpp->Location = System::Drawing::Point(10, 85);
            this->fcgLBWpp->Name = L"fcgLBWpp";
            this->fcgLBWpp->Size = System::Drawing::Size(123, 14);
            this->fcgLBWpp->TabIndex = 73;
            this->fcgLBWpp->Text = L"波面状並列処理 (WPP)";
            // 
            // fcgCBWpp
            // 
            this->fcgCBWpp->AutoSize = true;
            this->fcgCBWpp->Location = System::Drawing::Point(145, 87);
            this->fcgCBWpp->Name = L"fcgCBWpp";
            this->fcgCBWpp->Size = System::Drawing::Size(15, 14);
            this->fcgCBWpp->TabIndex = 43;
            this->fcgCBWpp->Tag = L"reCmd";
            this->fcgCBWpp->UseVisualStyleBackColor = true;
            // 
            // fcgLBFrameThreads
            // 
            this->fcgLBFrameThreads->AutoSize = true;
            this->fcgLBFrameThreads->Location = System::Drawing::Point(6, 51);
            this->fcgLBFrameThreads->Name = L"fcgLBFrameThreads";
            this->fcgLBFrameThreads->Size = System::Drawing::Size(73, 14);
            this->fcgLBFrameThreads->TabIndex = 2;
            this->fcgLBFrameThreads->Text = L"フレーム並列数";
            // 
            // fcgNUFrameThreads
            // 
            this->fcgNUFrameThreads->Location = System::Drawing::Point(95, 49);
            this->fcgNUFrameThreads->Name = L"fcgNUFrameThreads";
            this->fcgNUFrameThreads->Size = System::Drawing::Size(64, 21);
            this->fcgNUFrameThreads->TabIndex = 42;
            this->fcgNUFrameThreads->Tag = L"reCmd";
            this->fcgNUFrameThreads->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBThreads
            // 
            this->fcgLBThreads->AutoSize = true;
            this->fcgLBThreads->Location = System::Drawing::Point(31, 21);
            this->fcgLBThreads->Name = L"fcgLBThreads";
            this->fcgLBThreads->Size = System::Drawing::Size(48, 14);
            this->fcgLBThreads->TabIndex = 0;
            this->fcgLBThreads->Text = L"スレッド数";
            // 
            // fcgNUPools
            // 
            this->fcgNUPools->Location = System::Drawing::Point(95, 19);
            this->fcgNUPools->Name = L"fcgNUPools";
            this->fcgNUPools->Size = System::Drawing::Size(64, 21);
            this->fcgNUPools->TabIndex = 41;
            this->fcgNUPools->Tag = L"reCmd";
            this->fcgNUPools->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgCXInterlaced
            // 
            this->fcgCXInterlaced->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXInterlaced->FormattingEnabled = true;
            this->fcgCXInterlaced->Location = System::Drawing::Point(493, 133);
            this->fcgCXInterlaced->Name = L"fcgCXInterlaced";
            this->fcgCXInterlaced->Size = System::Drawing::Size(96, 22);
            this->fcgCXInterlaced->TabIndex = 55;
            this->fcgCXInterlaced->Tag = L"reCmd";
            // 
            // fcgLBInterlaced
            // 
            this->fcgLBInterlaced->AutoSize = true;
            this->fcgLBInterlaced->Location = System::Drawing::Point(412, 136);
            this->fcgLBInterlaced->Name = L"fcgLBInterlaced";
            this->fcgLBInterlaced->Size = System::Drawing::Size(61, 14);
            this->fcgLBInterlaced->TabIndex = 70;
            this->fcgLBInterlaced->Text = L"インタレ保持";
            // 
            // fcggroupBoxVUIColor
            // 
            this->fcggroupBoxVUIColor->Controls->Add(this->fcgCBFullRange);
            this->fcggroupBoxVUIColor->Controls->Add(this->fcgLBFullRange);
            this->fcggroupBoxVUIColor->Controls->Add(this->fcgCXTransfer);
            this->fcggroupBoxVUIColor->Controls->Add(this->fcgCXColorPrim);
            this->fcggroupBoxVUIColor->Controls->Add(this->fcgCXColorMatrix);
            this->fcggroupBoxVUIColor->Controls->Add(this->fcgLBTransfer);
            this->fcggroupBoxVUIColor->Controls->Add(this->fcgLBColorPrim);
            this->fcggroupBoxVUIColor->Controls->Add(this->fcgLBColorMatrix);
            this->fcggroupBoxVUIColor->Location = System::Drawing::Point(392, 348);
            this->fcggroupBoxVUIColor->Name = L"fcggroupBoxVUIColor";
            this->fcggroupBoxVUIColor->Size = System::Drawing::Size(209, 140);
            this->fcggroupBoxVUIColor->TabIndex = 70;
            this->fcggroupBoxVUIColor->TabStop = false;
            this->fcggroupBoxVUIColor->Text = L"色空間";
            // 
            // fcgCBFullRange
            // 
            this->fcgCBFullRange->AutoSize = true;
            this->fcgCBFullRange->Location = System::Drawing::Point(164, 115);
            this->fcgCBFullRange->Name = L"fcgCBFullRange";
            this->fcgCBFullRange->Size = System::Drawing::Size(15, 14);
            this->fcgCBFullRange->TabIndex = 74;
            this->fcgCBFullRange->Tag = L"reCmd";
            this->fcgCBFullRange->UseVisualStyleBackColor = true;
            // 
            // fcgLBFullRange
            // 
            this->fcgLBFullRange->AutoSize = true;
            this->fcgLBFullRange->Location = System::Drawing::Point(18, 114);
            this->fcgLBFullRange->Name = L"fcgLBFullRange";
            this->fcgLBFullRange->Size = System::Drawing::Size(70, 14);
            this->fcgLBFullRange->TabIndex = 3;
            this->fcgLBFullRange->Text = L"input range";
            // 
            // fcgCXTransfer
            // 
            this->fcgCXTransfer->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXTransfer->FormattingEnabled = true;
            this->fcgCXTransfer->Location = System::Drawing::Point(105, 81);
            this->fcgCXTransfer->Name = L"fcgCXTransfer";
            this->fcgCXTransfer->Size = System::Drawing::Size(92, 22);
            this->fcgCXTransfer->TabIndex = 73;
            this->fcgCXTransfer->Tag = L"reCmd";
            // 
            // fcgCXColorPrim
            // 
            this->fcgCXColorPrim->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXColorPrim->FormattingEnabled = true;
            this->fcgCXColorPrim->Location = System::Drawing::Point(105, 49);
            this->fcgCXColorPrim->Name = L"fcgCXColorPrim";
            this->fcgCXColorPrim->Size = System::Drawing::Size(92, 22);
            this->fcgCXColorPrim->TabIndex = 72;
            this->fcgCXColorPrim->Tag = L"reCmd";
            // 
            // fcgCXColorMatrix
            // 
            this->fcgCXColorMatrix->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXColorMatrix->FormattingEnabled = true;
            this->fcgCXColorMatrix->Location = System::Drawing::Point(105, 18);
            this->fcgCXColorMatrix->Name = L"fcgCXColorMatrix";
            this->fcgCXColorMatrix->Size = System::Drawing::Size(92, 22);
            this->fcgCXColorMatrix->TabIndex = 71;
            this->fcgCXColorMatrix->Tag = L"reCmd";
            // 
            // fcgLBTransfer
            // 
            this->fcgLBTransfer->AutoSize = true;
            this->fcgLBTransfer->Location = System::Drawing::Point(18, 84);
            this->fcgLBTransfer->Name = L"fcgLBTransfer";
            this->fcgLBTransfer->Size = System::Drawing::Size(49, 14);
            this->fcgLBTransfer->TabIndex = 2;
            this->fcgLBTransfer->Text = L"transfer";
            // 
            // fcgLBColorPrim
            // 
            this->fcgLBColorPrim->AutoSize = true;
            this->fcgLBColorPrim->Location = System::Drawing::Point(18, 52);
            this->fcgLBColorPrim->Name = L"fcgLBColorPrim";
            this->fcgLBColorPrim->Size = System::Drawing::Size(61, 14);
            this->fcgLBColorPrim->TabIndex = 1;
            this->fcgLBColorPrim->Text = L"colorprim";
            // 
            // fcgLBColorMatrix
            // 
            this->fcgLBColorMatrix->AutoSize = true;
            this->fcgLBColorMatrix->Location = System::Drawing::Point(18, 21);
            this->fcgLBColorMatrix->Name = L"fcgLBColorMatrix";
            this->fcgLBColorMatrix->Size = System::Drawing::Size(70, 14);
            this->fcgLBColorMatrix->TabIndex = 0;
            this->fcgLBColorMatrix->Text = L"colormatrix";
            // 
            // fcgCXVideoFormat
            // 
            this->fcgCXVideoFormat->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXVideoFormat->FormattingEnabled = true;
            this->fcgCXVideoFormat->Location = System::Drawing::Point(493, 102);
            this->fcgCXVideoFormat->Name = L"fcgCXVideoFormat";
            this->fcgCXVideoFormat->Size = System::Drawing::Size(95, 22);
            this->fcgCXVideoFormat->TabIndex = 54;
            this->fcgCXVideoFormat->Tag = L"reCmd";
            // 
            // fcgLBVideoFormat
            // 
            this->fcgLBVideoFormat->AutoSize = true;
            this->fcgLBVideoFormat->Location = System::Drawing::Point(408, 106);
            this->fcgLBVideoFormat->Name = L"fcgLBVideoFormat";
            this->fcgLBVideoFormat->Size = System::Drawing::Size(73, 14);
            this->fcgLBVideoFormat->TabIndex = 67;
            this->fcgLBVideoFormat->Text = L"videoformat";
            // 
            // fcgGroupBoxAspectRatio
            // 
            this->fcgGroupBoxAspectRatio->Controls->Add(this->fcgNUAspectRatioY);
            this->fcgGroupBoxAspectRatio->Controls->Add(this->fcgLBAspectRatio);
            this->fcgGroupBoxAspectRatio->Controls->Add(this->fcgNUAspectRatioX);
            this->fcgGroupBoxAspectRatio->Controls->Add(this->fcgCXAspectRatio);
            this->fcgGroupBoxAspectRatio->Location = System::Drawing::Point(396, 5);
            this->fcgGroupBoxAspectRatio->Name = L"fcgGroupBoxAspectRatio";
            this->fcgGroupBoxAspectRatio->Size = System::Drawing::Size(205, 86);
            this->fcgGroupBoxAspectRatio->TabIndex = 50;
            this->fcgGroupBoxAspectRatio->TabStop = false;
            this->fcgGroupBoxAspectRatio->Text = L"アスペクト比";
            // 
            // fcgNUAspectRatioY
            // 
            this->fcgNUAspectRatioY->Location = System::Drawing::Point(126, 53);
            this->fcgNUAspectRatioY->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000, 0, 0, 0 });
            this->fcgNUAspectRatioY->Name = L"fcgNUAspectRatioY";
            this->fcgNUAspectRatioY->Size = System::Drawing::Size(60, 21);
            this->fcgNUAspectRatioY->TabIndex = 53;
            this->fcgNUAspectRatioY->Tag = L"reCmd";
            this->fcgNUAspectRatioY->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBAspectRatio
            // 
            this->fcgLBAspectRatio->AutoSize = true;
            this->fcgLBAspectRatio->Location = System::Drawing::Point(108, 55);
            this->fcgLBAspectRatio->Name = L"fcgLBAspectRatio";
            this->fcgLBAspectRatio->Size = System::Drawing::Size(12, 14);
            this->fcgLBAspectRatio->TabIndex = 2;
            this->fcgLBAspectRatio->Text = L":";
            // 
            // fcgNUAspectRatioX
            // 
            this->fcgNUAspectRatioX->Location = System::Drawing::Point(42, 53);
            this->fcgNUAspectRatioX->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000, 0, 0, 0 });
            this->fcgNUAspectRatioX->Name = L"fcgNUAspectRatioX";
            this->fcgNUAspectRatioX->Size = System::Drawing::Size(60, 21);
            this->fcgNUAspectRatioX->TabIndex = 52;
            this->fcgNUAspectRatioX->Tag = L"reCmd";
            this->fcgNUAspectRatioX->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgCXAspectRatio
            // 
            this->fcgCXAspectRatio->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXAspectRatio->FormattingEnabled = true;
            this->fcgCXAspectRatio->Location = System::Drawing::Point(15, 25);
            this->fcgCXAspectRatio->Name = L"fcgCXAspectRatio";
            this->fcgCXAspectRatio->Size = System::Drawing::Size(171, 22);
            this->fcgCXAspectRatio->TabIndex = 51;
            this->fcgCXAspectRatio->Tag = L"reCmd";
            // 
            // fcgCXCSP
            // 
            this->fcgCXCSP->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXCSP->FormattingEnabled = true;
            this->fcgCXCSP->Location = System::Drawing::Point(494, 315);
            this->fcgCXCSP->Name = L"fcgCXCSP";
            this->fcgCXCSP->Size = System::Drawing::Size(95, 22);
            this->fcgCXCSP->TabIndex = 66;
            this->fcgCXCSP->Tag = L"reCmd";
            // 
            // fcgLBCSP
            // 
            this->fcgLBCSP->AutoSize = true;
            this->fcgLBCSP->Location = System::Drawing::Point(396, 318);
            this->fcgLBCSP->Name = L"fcgLBCSP";
            this->fcgLBCSP->Size = System::Drawing::Size(85, 14);
            this->fcgLBCSP->TabIndex = 60;
            this->fcgLBCSP->Text = L"出力色フォーマット";
            // 
            // fcggroupBoxRateControl
            // 
            this->fcggroupBoxRateControl->Controls->Add(this->fcgLBQComp);
            this->fcggroupBoxRateControl->Controls->Add(this->fcgNUQComp);
            this->fcggroupBoxRateControl->Controls->Add(this->fcgLBVBVmax);
            this->fcggroupBoxRateControl->Controls->Add(this->fcgNUVBVbuf);
            this->fcggroupBoxRateControl->Controls->Add(this->fcgLBVBVbuf);
            this->fcggroupBoxRateControl->Controls->Add(this->fcgNUVBVmax);
            this->fcggroupBoxRateControl->Location = System::Drawing::Point(396, 164);
            this->fcggroupBoxRateControl->Name = L"fcggroupBoxRateControl";
            this->fcggroupBoxRateControl->Size = System::Drawing::Size(205, 108);
            this->fcggroupBoxRateControl->TabIndex = 60;
            this->fcggroupBoxRateControl->TabStop = false;
            this->fcggroupBoxRateControl->Text = L"レート制御";
            // 
            // fcgLBQComp
            // 
            this->fcgLBQComp->AutoSize = true;
            this->fcgLBQComp->Location = System::Drawing::Point(7, 79);
            this->fcgLBQComp->Name = L"fcgLBQComp";
            this->fcgLBQComp->Size = System::Drawing::Size(108, 14);
            this->fcgLBQComp->TabIndex = 60;
            this->fcgLBQComp->Text = L"ビットレート変動率(%)";
            // 
            // fcgNUQComp
            // 
            this->fcgNUQComp->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 0 });
            this->fcgNUQComp->Location = System::Drawing::Point(126, 77);
            this->fcgNUQComp->Name = L"fcgNUQComp";
            this->fcgNUQComp->Size = System::Drawing::Size(67, 21);
            this->fcgNUQComp->TabIndex = 63;
            this->fcgNUQComp->Tag = L"reCmd";
            this->fcgNUQComp->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBVBVmax
            // 
            this->fcgLBVBVmax->AutoSize = true;
            this->fcgLBVBVmax->Location = System::Drawing::Point(6, 22);
            this->fcgLBVBVmax->Name = L"fcgLBVBVmax";
            this->fcgLBVBVmax->Size = System::Drawing::Size(76, 14);
            this->fcgLBVBVmax->TabIndex = 56;
            this->fcgLBVBVmax->Text = L"最大ビットレート";
            // 
            // fcgNUVBVbuf
            // 
            this->fcgNUVBVbuf->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 500, 0, 0, 0 });
            this->fcgNUVBVbuf->Location = System::Drawing::Point(97, 49);
            this->fcgNUVBVbuf->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000000, 0, 0, 0 });
            this->fcgNUVBVbuf->Name = L"fcgNUVBVbuf";
            this->fcgNUVBVbuf->Size = System::Drawing::Size(96, 21);
            this->fcgNUVBVbuf->TabIndex = 62;
            this->fcgNUVBVbuf->Tag = L"reCmd";
            this->fcgNUVBVbuf->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBVBVbuf
            // 
            this->fcgLBVBVbuf->AutoSize = true;
            this->fcgLBVBVbuf->Location = System::Drawing::Point(7, 51);
            this->fcgLBVBVbuf->Name = L"fcgLBVBVbuf";
            this->fcgLBVBVbuf->Size = System::Drawing::Size(61, 14);
            this->fcgLBVBVbuf->TabIndex = 58;
            this->fcgLBVBVbuf->Text = L"バッファサイズ";
            // 
            // fcgNUVBVmax
            // 
            this->fcgNUVBVmax->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 500, 0, 0, 0 });
            this->fcgNUVBVmax->Location = System::Drawing::Point(97, 20);
            this->fcgNUVBVmax->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1000000, 0, 0, 0 });
            this->fcgNUVBVmax->Name = L"fcgNUVBVmax";
            this->fcgNUVBVmax->Size = System::Drawing::Size(95, 21);
            this->fcgNUVBVmax->TabIndex = 61;
            this->fcgNUVBVmax->Tag = L"reCmd";
            this->fcgNUVBVmax->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcggroupBoxPreset
            // 
            this->fcggroupBoxPreset->Controls->Add(this->fcgBTApplyPreset);
            this->fcggroupBoxPreset->Controls->Add(this->fcgCXProfile);
            this->fcggroupBoxPreset->Controls->Add(this->fcgCXTune);
            this->fcggroupBoxPreset->Controls->Add(this->fcgCXPreset);
            this->fcggroupBoxPreset->Controls->Add(this->fcgLBProfile);
            this->fcggroupBoxPreset->Controls->Add(this->fcgLBX265Tune);
            this->fcggroupBoxPreset->Controls->Add(this->fcgLBX265Preset);
            this->fcggroupBoxPreset->Location = System::Drawing::Point(7, 305);
            this->fcggroupBoxPreset->Name = L"fcggroupBoxPreset";
            this->fcggroupBoxPreset->Size = System::Drawing::Size(182, 183);
            this->fcggroupBoxPreset->TabIndex = 30;
            this->fcggroupBoxPreset->TabStop = false;
            this->fcggroupBoxPreset->Text = L"プリセットのロード";
            // 
            // fcgBTApplyPreset
            // 
            this->fcgBTApplyPreset->Location = System::Drawing::Point(73, 138);
            this->fcgBTApplyPreset->Name = L"fcgBTApplyPreset";
            this->fcgBTApplyPreset->Size = System::Drawing::Size(94, 32);
            this->fcgBTApplyPreset->TabIndex = 34;
            this->fcgBTApplyPreset->Text = L"GUIにロード";
            this->fcgBTApplyPreset->UseVisualStyleBackColor = true;
            this->fcgBTApplyPreset->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTApplyPreset_Click);
            // 
            // fcgCXProfile
            // 
            this->fcgCXProfile->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXProfile->FormattingEnabled = true;
            this->fcgCXProfile->Location = System::Drawing::Point(73, 98);
            this->fcgCXProfile->Name = L"fcgCXProfile";
            this->fcgCXProfile->Size = System::Drawing::Size(94, 22);
            this->fcgCXProfile->TabIndex = 33;
            this->fcgCXProfile->Tag = L"reCmd";
            // 
            // fcgCXTune
            // 
            this->fcgCXTune->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXTune->FormattingEnabled = true;
            this->fcgCXTune->Location = System::Drawing::Point(73, 62);
            this->fcgCXTune->Name = L"fcgCXTune";
            this->fcgCXTune->Size = System::Drawing::Size(94, 22);
            this->fcgCXTune->TabIndex = 32;
            this->fcgCXTune->Tag = L"reCmd";
            // 
            // fcgCXPreset
            // 
            this->fcgCXPreset->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXPreset->FormattingEnabled = true;
            this->fcgCXPreset->Location = System::Drawing::Point(73, 26);
            this->fcgCXPreset->Name = L"fcgCXPreset";
            this->fcgCXPreset->Size = System::Drawing::Size(94, 22);
            this->fcgCXPreset->TabIndex = 31;
            this->fcgCXPreset->Tag = L"reCmd";
            // 
            // fcgLBProfile
            // 
            this->fcgLBProfile->AutoSize = true;
            this->fcgLBProfile->Location = System::Drawing::Point(9, 101);
            this->fcgLBProfile->Name = L"fcgLBProfile";
            this->fcgLBProfile->Size = System::Drawing::Size(53, 14);
            this->fcgLBProfile->TabIndex = 2;
            this->fcgLBProfile->Text = L"プロファイル";
            // 
            // fcgLBX265Tune
            // 
            this->fcgLBX265Tune->AutoSize = true;
            this->fcgLBX265Tune->Location = System::Drawing::Point(9, 65);
            this->fcgLBX265Tune->Name = L"fcgLBX265Tune";
            this->fcgLBX265Tune->Size = System::Drawing::Size(56, 14);
            this->fcgLBX265Tune->TabIndex = 1;
            this->fcgLBX265Tune->Text = L"チューニング";
            // 
            // fcgLBX265Preset
            // 
            this->fcgLBX265Preset->AutoSize = true;
            this->fcgLBX265Preset->Location = System::Drawing::Point(9, 29);
            this->fcgLBX265Preset->Name = L"fcgLBX265Preset";
            this->fcgLBX265Preset->Size = System::Drawing::Size(29, 14);
            this->fcgLBX265Preset->TabIndex = 0;
            this->fcgLBX265Preset->Text = L"速度";
            // 
            // fcgPNStatusFile
            // 
            this->fcgPNStatusFile->Controls->Add(this->fcgLBSTATUS);
            this->fcgPNStatusFile->Controls->Add(this->fcgTXStatusFile);
            this->fcgPNStatusFile->Controls->Add(this->fcgBTStatusFile);
            this->fcgPNStatusFile->Location = System::Drawing::Point(5, 271);
            this->fcgPNStatusFile->Name = L"fcgPNStatusFile";
            this->fcgPNStatusFile->Size = System::Drawing::Size(376, 32);
            this->fcgPNStatusFile->TabIndex = 6;
            // 
            // fcgLBSTATUS
            // 
            this->fcgLBSTATUS->AutoSize = true;
            this->fcgLBSTATUS->Location = System::Drawing::Point(-1, 10);
            this->fcgLBSTATUS->Name = L"fcgLBSTATUS";
            this->fcgLBSTATUS->Size = System::Drawing::Size(78, 14);
            this->fcgLBSTATUS->TabIndex = 19;
            this->fcgLBSTATUS->Text = L"ステータスファイル";
            // 
            // fcgTXStatusFile
            // 
            this->fcgTXStatusFile->Location = System::Drawing::Point(94, 7);
            this->fcgTXStatusFile->Name = L"fcgTXStatusFile";
            this->fcgTXStatusFile->Size = System::Drawing::Size(253, 21);
            this->fcgTXStatusFile->TabIndex = 16;
            this->fcgTXStatusFile->Tag = L"reCmd";
            this->fcgTXStatusFile->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXStatusFile->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            // 
            // fcgBTStatusFile
            // 
            this->fcgBTStatusFile->Location = System::Drawing::Point(347, 6);
            this->fcgBTStatusFile->Name = L"fcgBTStatusFile";
            this->fcgBTStatusFile->Size = System::Drawing::Size(27, 22);
            this->fcgBTStatusFile->TabIndex = 17;
            this->fcgBTStatusFile->Text = L"...";
            this->fcgBTStatusFile->UseVisualStyleBackColor = true;
            this->fcgBTStatusFile->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTStatusFile_Click);
            // 
            // fcgPNBitrate
            // 
            this->fcgPNBitrate->Controls->Add(this->fcgLBQuality);
            this->fcgPNBitrate->Controls->Add(this->fcgLBQualityLeft);
            this->fcgPNBitrate->Controls->Add(this->fcgLBQualityRight);
            this->fcgPNBitrate->Controls->Add(this->fcgTXQuality);
            this->fcgPNBitrate->Controls->Add(this->fcgBTTBQualitySubtract);
            this->fcgPNBitrate->Controls->Add(this->fcgBTTBQualityAdd);
            this->fcgPNBitrate->Controls->Add(this->fcgTBQuality);
            this->fcgPNBitrate->Location = System::Drawing::Point(5, 191);
            this->fcgPNBitrate->Name = L"fcgPNBitrate";
            this->fcgPNBitrate->Size = System::Drawing::Size(376, 84);
            this->fcgPNBitrate->TabIndex = 5;
            // 
            // fcgLBQuality
            // 
            this->fcgLBQuality->AutoSize = true;
            this->fcgLBQuality->Location = System::Drawing::Point(3, 8);
            this->fcgLBQuality->Name = L"fcgLBQuality";
            this->fcgLBQuality->Size = System::Drawing::Size(89, 14);
            this->fcgLBQuality->TabIndex = 14;
            this->fcgLBQuality->Text = L"ビットレート(kbps)";
            // 
            // fcgLBQualityLeft
            // 
            this->fcgLBQualityLeft->AutoSize = true;
            this->fcgLBQualityLeft->Location = System::Drawing::Point(6, 62);
            this->fcgLBQualityLeft->Name = L"fcgLBQualityLeft";
            this->fcgLBQualityLeft->Size = System::Drawing::Size(40, 14);
            this->fcgLBQualityLeft->TabIndex = 12;
            this->fcgLBQualityLeft->Text = L"低画質";
            // 
            // fcgLBQualityRight
            // 
            this->fcgLBQualityRight->AutoSize = true;
            this->fcgLBQualityRight->Location = System::Drawing::Point(322, 62);
            this->fcgLBQualityRight->Name = L"fcgLBQualityRight";
            this->fcgLBQualityRight->Size = System::Drawing::Size(40, 14);
            this->fcgLBQualityRight->TabIndex = 13;
            this->fcgLBQualityRight->Text = L"高画質";
            // 
            // fcgTXQuality
            // 
            this->fcgTXQuality->Location = System::Drawing::Point(290, 8);
            this->fcgTXQuality->Name = L"fcgTXQuality";
            this->fcgTXQuality->Size = System::Drawing::Size(76, 21);
            this->fcgTXQuality->TabIndex = 12;
            this->fcgTXQuality->Tag = L"reCmd";
            this->fcgTXQuality->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgTXQuality->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTXQuality_TextChanged);
            this->fcgTXQuality->Enter += gcnew System::EventHandler(this, &frmConfig::fcgTXQuality_Enter);
            this->fcgTXQuality->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmConfig::fcgTXQuality_Validating);
            // 
            // fcgBTTBQualitySubtract
            // 
            this->fcgBTTBQualitySubtract->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"fcgBTTBQualitySubtract.Image")));
            this->fcgBTTBQualitySubtract->Location = System::Drawing::Point(0, 33);
            this->fcgBTTBQualitySubtract->Name = L"fcgBTTBQualitySubtract";
            this->fcgBTTBQualitySubtract->Size = System::Drawing::Size(21, 23);
            this->fcgBTTBQualitySubtract->TabIndex = 13;
            this->fcgBTTBQualitySubtract->UseVisualStyleBackColor = true;
            this->fcgBTTBQualitySubtract->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTTBQualitySubtract_Click);
            this->fcgBTTBQualitySubtract->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &frmConfig::fcgBTTBQualitySubtract_MouseDown);
            this->fcgBTTBQualitySubtract->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &frmConfig::fcgBTTBQualitySubtract_MouseUp);
            // 
            // fcgBTTBQualityAdd
            // 
            this->fcgBTTBQualityAdd->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"fcgBTTBQualityAdd.Image")));
            this->fcgBTTBQualityAdd->Location = System::Drawing::Point(352, 34);
            this->fcgBTTBQualityAdd->Name = L"fcgBTTBQualityAdd";
            this->fcgBTTBQualityAdd->Size = System::Drawing::Size(21, 23);
            this->fcgBTTBQualityAdd->TabIndex = 15;
            this->fcgBTTBQualityAdd->UseVisualStyleBackColor = true;
            this->fcgBTTBQualityAdd->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTTBQualityAdd_Click);
            this->fcgBTTBQualityAdd->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &frmConfig::fcgBTTBQualityAdd_MouseDown);
            this->fcgBTTBQualityAdd->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &frmConfig::fcgBTTBQualityAdd_MouseUp);
            // 
            // fcgTBQuality
            // 
            this->fcgTBQuality->AutoSize = false;
            this->fcgTBQuality->BackColor = System::Drawing::SystemColors::Window;
            this->fcgTBQuality->Location = System::Drawing::Point(15, 35);
            this->fcgTBQuality->Maximum = 54000;
            this->fcgTBQuality->Name = L"fcgTBQuality";
            this->fcgTBQuality->Size = System::Drawing::Size(339, 25);
            this->fcgTBQuality->TabIndex = 14;
            this->fcgTBQuality->TickStyle = System::Windows::Forms::TickStyle::None;
            this->fcgTBQuality->Scroll += gcnew System::EventHandler(this, &frmConfig::fcgTBQuality_Scroll);
            this->fcgTBQuality->ValueChanged += gcnew System::EventHandler(this, &frmConfig::fcgTBQuality_ValueChanged);
            // 
            // fcgPNX265Mode
            // 
            this->fcgPNX265Mode->Controls->Add(this->fcgNUAMPLimitBitrate);
            this->fcgPNX265Mode->Controls->Add(this->fcgNUAMPLimitFileSize);
            this->fcgPNX265Mode->Controls->Add(this->fcgCXX265Mode);
            this->fcgPNX265Mode->Controls->Add(this->fcgCBNulOut);
            this->fcgPNX265Mode->Controls->Add(this->fcgCBAMPLimitBitrate);
            this->fcgPNX265Mode->Controls->Add(this->fcgCBFastFirstPass);
            this->fcgPNX265Mode->Controls->Add(this->fcgNUAutoNPass);
            this->fcgPNX265Mode->Controls->Add(this->fcgCBAMPLimitFileSize);
            this->fcgPNX265Mode->Controls->Add(this->fcgLBAutoNpass);
            this->fcgPNX265Mode->Location = System::Drawing::Point(5, 67);
            this->fcgPNX265Mode->Name = L"fcgPNX265Mode";
            this->fcgPNX265Mode->Size = System::Drawing::Size(376, 126);
            this->fcgPNX265Mode->TabIndex = 4;
            // 
            // fcgNUAMPLimitBitrate
            // 
            this->fcgNUAMPLimitBitrate->DecimalPlaces = 1;
            this->fcgNUAMPLimitBitrate->Location = System::Drawing::Point(290, 100);
            this->fcgNUAMPLimitBitrate->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 65536000, 0, 0, 0 });
            this->fcgNUAMPLimitBitrate->Name = L"fcgNUAMPLimitBitrate";
            this->fcgNUAMPLimitBitrate->Size = System::Drawing::Size(78, 21);
            this->fcgNUAMPLimitBitrate->TabIndex = 11;
            this->fcgNUAMPLimitBitrate->Tag = L"chValue";
            this->fcgNUAMPLimitBitrate->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgNUAMPLimitFileSize
            // 
            this->fcgNUAMPLimitFileSize->DecimalPlaces = 1;
            this->fcgNUAMPLimitFileSize->Location = System::Drawing::Point(290, 75);
            this->fcgNUAMPLimitFileSize->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 16384, 0, 0, 0 });
            this->fcgNUAMPLimitFileSize->Name = L"fcgNUAMPLimitFileSize";
            this->fcgNUAMPLimitFileSize->Size = System::Drawing::Size(78, 21);
            this->fcgNUAMPLimitFileSize->TabIndex = 9;
            this->fcgNUAMPLimitFileSize->Tag = L"chValue";
            this->fcgNUAMPLimitFileSize->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgCXX265Mode
            // 
            this->fcgCXX265Mode->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXX265Mode->FormattingEnabled = true;
            this->fcgCXX265Mode->Location = System::Drawing::Point(6, 7);
            this->fcgCXX265Mode->Name = L"fcgCXX265Mode";
            this->fcgCXX265Mode->Size = System::Drawing::Size(362, 22);
            this->fcgCXX265Mode->TabIndex = 4;
            this->fcgCXX265Mode->Tag = L"reCmd";
            this->fcgCXX265Mode->SelectedIndexChanged += gcnew System::EventHandler(this, &frmConfig::fcgCXX265Mode_SelectedIndexChanged);
            // 
            // fcgCBNulOut
            // 
            this->fcgCBNulOut->AutoSize = true;
            this->fcgCBNulOut->Location = System::Drawing::Point(6, 45);
            this->fcgCBNulOut->Name = L"fcgCBNulOut";
            this->fcgCBNulOut->Size = System::Drawing::Size(65, 18);
            this->fcgCBNulOut->TabIndex = 5;
            this->fcgCBNulOut->Tag = L"reCmd";
            this->fcgCBNulOut->Text = L"nul出力";
            this->fcgCBNulOut->UseVisualStyleBackColor = true;
            this->fcgCBNulOut->CheckedChanged += gcnew System::EventHandler(this, &frmConfig::fcgCBNulOut_CheckedChanged);
            // 
            // fcgCBAMPLimitBitrate
            // 
            this->fcgCBAMPLimitBitrate->AutoSize = true;
            this->fcgCBAMPLimitBitrate->Location = System::Drawing::Point(30, 101);
            this->fcgCBAMPLimitBitrate->Name = L"fcgCBAMPLimitBitrate";
            this->fcgCBAMPLimitBitrate->Size = System::Drawing::Size(221, 18);
            this->fcgCBAMPLimitBitrate->TabIndex = 10;
            this->fcgCBAMPLimitBitrate->Tag = L"chValue";
            this->fcgCBAMPLimitBitrate->Text = L"上限ファイルビットレート(映像+音声, kbps)";
            this->fcgCBAMPLimitBitrate->UseVisualStyleBackColor = true;
            // 
            // fcgCBFastFirstPass
            // 
            this->fcgCBFastFirstPass->AutoSize = true;
            this->fcgCBFastFirstPass->Location = System::Drawing::Point(86, 45);
            this->fcgCBFastFirstPass->Name = L"fcgCBFastFirstPass";
            this->fcgCBFastFirstPass->Size = System::Drawing::Size(101, 18);
            this->fcgCBFastFirstPass->TabIndex = 6;
            this->fcgCBFastFirstPass->Tag = L"reCmd";
            this->fcgCBFastFirstPass->Text = L"高速(1st pass)";
            this->fcgCBFastFirstPass->UseVisualStyleBackColor = true;
            this->fcgCBFastFirstPass->CheckedChanged += gcnew System::EventHandler(this, &frmConfig::fcgCBFastFirstPass_CheckedChanged);
            // 
            // fcgNUAutoNPass
            // 
            this->fcgNUAutoNPass->Location = System::Drawing::Point(313, 44);
            this->fcgNUAutoNPass->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
            this->fcgNUAutoNPass->Name = L"fcgNUAutoNPass";
            this->fcgNUAutoNPass->Size = System::Drawing::Size(55, 21);
            this->fcgNUAutoNPass->TabIndex = 7;
            this->fcgNUAutoNPass->Tag = L"reCmd";
            this->fcgNUAutoNPass->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNUAutoNPass->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
            // 
            // fcgCBAMPLimitFileSize
            // 
            this->fcgCBAMPLimitFileSize->AutoSize = true;
            this->fcgCBAMPLimitFileSize->Location = System::Drawing::Point(30, 76);
            this->fcgCBAMPLimitFileSize->Name = L"fcgCBAMPLimitFileSize";
            this->fcgCBAMPLimitFileSize->Size = System::Drawing::Size(191, 18);
            this->fcgCBAMPLimitFileSize->TabIndex = 8;
            this->fcgCBAMPLimitFileSize->Tag = L"chValue";
            this->fcgCBAMPLimitFileSize->Text = L"上限ファイルサイズ(映像+音声, MB)";
            this->fcgCBAMPLimitFileSize->UseVisualStyleBackColor = true;
            // 
            // fcgLBAutoNpass
            // 
            this->fcgLBAutoNpass->AutoSize = true;
            this->fcgLBAutoNpass->Location = System::Drawing::Point(218, 46);
            this->fcgLBAutoNpass->Name = L"fcgLBAutoNpass";
            this->fcgLBAutoNpass->Size = System::Drawing::Size(83, 14);
            this->fcgLBAutoNpass->TabIndex = 34;
            this->fcgLBAutoNpass->Text = L"自動マルチパス数";
            // 
            // fcgpictureBoxX265
            // 
            this->fcgpictureBoxX265->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"fcgpictureBoxX265.Image")));
            this->fcgpictureBoxX265->Location = System::Drawing::Point(4, 12);
            this->fcgpictureBoxX265->Name = L"fcgpictureBoxX265";
            this->fcgpictureBoxX265->Size = System::Drawing::Size(110, 44);
            this->fcgpictureBoxX265->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
            this->fcgpictureBoxX265->TabIndex = 16;
            this->fcgpictureBoxX265->TabStop = false;
            // 
            // fcgBTX265Path
            // 
            this->fcgBTX265Path->Location = System::Drawing::Point(354, 34);
            this->fcgBTX265Path->Name = L"fcgBTX265Path";
            this->fcgBTX265Path->Size = System::Drawing::Size(27, 22);
            this->fcgBTX265Path->TabIndex = 1;
            this->fcgBTX265Path->Text = L"...";
            this->fcgBTX265Path->UseVisualStyleBackColor = true;
            this->fcgBTX265Path->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTX265Path_Click);
            // 
            // fcgTXX265Path
            // 
            this->fcgTXX265Path->AllowDrop = true;
            this->fcgTXX265Path->Location = System::Drawing::Point(119, 35);
            this->fcgTXX265Path->Name = L"fcgTXX265Path";
            this->fcgTXX265Path->Size = System::Drawing::Size(235, 21);
            this->fcgTXX265Path->TabIndex = 0;
            this->fcgTXX265Path->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTXX265Path_TextChanged);
            this->fcgTXX265Path->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXX265Path->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            this->fcgTXX265Path->Enter += gcnew System::EventHandler(this, &frmConfig::fcgTXX265Path_Enter);
            this->fcgTXX265Path->Leave += gcnew System::EventHandler(this, &frmConfig::fcgTXX265Path_Leave);
            // 
            // fcgLBX265Path
            // 
            this->fcgLBX265Path->AutoSize = true;
            this->fcgLBX265Path->Location = System::Drawing::Point(128, 12);
            this->fcgLBX265Path->Name = L"fcgLBX265Path";
            this->fcgLBX265Path->Size = System::Drawing::Size(49, 14);
            this->fcgLBX265Path->TabIndex = 0;
            this->fcgLBX265Path->Text = L"～の指定";
            // 
            // fcgtabPageX265Other
            // 
            this->fcgtabPageX265Other->Controls->Add(this->fcgCBHEVCAQ);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBHEVCAQ);
            this->fcgtabPageX265Other->Controls->Add(this->fcgCBSsimRd);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBSsimRd);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBAQMotion);
            this->fcgtabPageX265Other->Controls->Add(this->fcgCBAQMotion);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBQGSize);
            this->fcgtabPageX265Other->Controls->Add(this->fcgNUQGSize);
            this->fcgtabPageX265Other->Controls->Add(this->fcgNURdoqLevel);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBRdoqLevel);
            this->fcgtabPageX265Other->Controls->Add(this->fcgCBDeblock);
            this->fcgtabPageX265Other->Controls->Add(this->fcggroupBoxDeblock);
            this->fcgtabPageX265Other->Controls->Add(this->fcgNUPsyRDOQ);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBPsyRDOQ);
            this->fcgtabPageX265Other->Controls->Add(this->fcgNUPsyRD);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBPsyRD);
            this->fcgtabPageX265Other->Controls->Add(this->fcggroupBoxFrame);
            this->fcgtabPageX265Other->Controls->Add(this->fcgCBCUTree);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBCUTree);
            this->fcgtabPageX265Other->Controls->Add(this->fcgNURD);
            this->fcgtabPageX265Other->Controls->Add(this->fcgNUAQStrength);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBAQStrength);
            this->fcgtabPageX265Other->Controls->Add(this->fcgCXAQMode);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBAQMode);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBRD);
            this->fcgtabPageX265Other->Controls->Add(this->fcgGroupBoxME);
            this->fcgtabPageX265Other->Controls->Add(this->fcgGroupBoxCTU);
            this->fcgtabPageX265Other->Controls->Add(this->fcgCBSAO);
            this->fcgtabPageX265Other->Controls->Add(this->fcgLBSAO);
            this->fcgtabPageX265Other->Location = System::Drawing::Point(4, 23);
            this->fcgtabPageX265Other->Name = L"fcgtabPageX265Other";
            this->fcgtabPageX265Other->Size = System::Drawing::Size(608, 493);
            this->fcgtabPageX265Other->TabIndex = 6;
            this->fcgtabPageX265Other->Text = L"x265 (2)";
            this->fcgtabPageX265Other->UseVisualStyleBackColor = true;
            // 
            // fcgCBHEVCAQ
            // 
            this->fcgCBHEVCAQ->AutoSize = true;
            this->fcgCBHEVCAQ->Location = System::Drawing::Point(102, 437);
            this->fcgCBHEVCAQ->Name = L"fcgCBHEVCAQ";
            this->fcgCBHEVCAQ->Size = System::Drawing::Size(15, 14);
            this->fcgCBHEVCAQ->TabIndex = 116;
            this->fcgCBHEVCAQ->Tag = L"reCmd";
            this->fcgCBHEVCAQ->UseVisualStyleBackColor = true;
            // 
            // fcgLBHEVCAQ
            // 
            this->fcgLBHEVCAQ->AutoSize = true;
            this->fcgLBHEVCAQ->Location = System::Drawing::Point(6, 436);
            this->fcgLBHEVCAQ->Name = L"fcgLBHEVCAQ";
            this->fcgLBHEVCAQ->Size = System::Drawing::Size(50, 14);
            this->fcgLBHEVCAQ->TabIndex = 117;
            this->fcgLBHEVCAQ->Text = L"hevc-aq";
            // 
            // fcgCBSsimRd
            // 
            this->fcgCBSsimRd->AutoSize = true;
            this->fcgCBSsimRd->Location = System::Drawing::Point(414, 466);
            this->fcgCBSsimRd->Name = L"fcgCBSsimRd";
            this->fcgCBSsimRd->Size = System::Drawing::Size(15, 14);
            this->fcgCBSsimRd->TabIndex = 54;
            this->fcgCBSsimRd->Tag = L"reCmd";
            this->fcgCBSsimRd->UseVisualStyleBackColor = true;
            // 
            // fcgLBSsimRd
            // 
            this->fcgLBSsimRd->AutoSize = true;
            this->fcgLBSsimRd->Location = System::Drawing::Point(255, 465);
            this->fcgLBSsimRd->Name = L"fcgLBSsimRd";
            this->fcgLBSsimRd->Size = System::Drawing::Size(48, 14);
            this->fcgLBSsimRd->TabIndex = 115;
            this->fcgLBSsimRd->Text = L"ssim-rd";
            // 
            // fcgLBAQMotion
            // 
            this->fcgLBAQMotion->AutoSize = true;
            this->fcgLBAQMotion->Location = System::Drawing::Point(6, 412);
            this->fcgLBAQMotion->Name = L"fcgLBAQMotion";
            this->fcgLBAQMotion->Size = System::Drawing::Size(63, 14);
            this->fcgLBAQMotion->TabIndex = 113;
            this->fcgLBAQMotion->Text = L"動き適応AQ";
            // 
            // fcgCBAQMotion
            // 
            this->fcgCBAQMotion->AutoSize = true;
            this->fcgCBAQMotion->Location = System::Drawing::Point(102, 413);
            this->fcgCBAQMotion->Name = L"fcgCBAQMotion";
            this->fcgCBAQMotion->Size = System::Drawing::Size(15, 14);
            this->fcgCBAQMotion->TabIndex = 42;
            this->fcgCBAQMotion->Tag = L"reCmd";
            this->fcgCBAQMotion->UseVisualStyleBackColor = true;
            // 
            // fcgLBQGSize
            // 
            this->fcgLBQGSize->AutoSize = true;
            this->fcgLBQGSize->Location = System::Drawing::Point(6, 465);
            this->fcgLBQGSize->Name = L"fcgLBQGSize";
            this->fcgLBQGSize->Size = System::Drawing::Size(96, 14);
            this->fcgLBQGSize->TabIndex = 111;
            this->fcgLBQGSize->Text = L"量子化ブロックサイズ";
            // 
            // fcgNUQGSize
            // 
            this->fcgNUQGSize->Location = System::Drawing::Point(102, 463);
            this->fcgNUQGSize->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 64, 0, 0, 0 });
            this->fcgNUQGSize->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 8, 0, 0, 0 });
            this->fcgNUQGSize->Name = L"fcgNUQGSize";
            this->fcgNUQGSize->Size = System::Drawing::Size(59, 21);
            this->fcgNUQGSize->TabIndex = 44;
            this->fcgNUQGSize->Tag = L"reCmd";
            this->fcgNUQGSize->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNUQGSize->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 16, 0, 0, 0 });
            this->fcgNUQGSize->ValueChanged += gcnew System::EventHandler(this, &frmConfig::fcgNUCtu_ValueChanged);
            // 
            // fcgNURdoqLevel
            // 
            this->fcgNURdoqLevel->Location = System::Drawing::Point(397, 408);
            this->fcgNURdoqLevel->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
            this->fcgNURdoqLevel->Name = L"fcgNURdoqLevel";
            this->fcgNURdoqLevel->Size = System::Drawing::Size(45, 21);
            this->fcgNURdoqLevel->TabIndex = 52;
            this->fcgNURdoqLevel->Tag = L"reCmd";
            this->fcgNURdoqLevel->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBRdoqLevel
            // 
            this->fcgLBRdoqLevel->AutoSize = true;
            this->fcgLBRdoqLevel->Location = System::Drawing::Point(255, 410);
            this->fcgLBRdoqLevel->Name = L"fcgLBRdoqLevel";
            this->fcgLBRdoqLevel->Size = System::Drawing::Size(62, 14);
            this->fcgLBRdoqLevel->TabIndex = 109;
            this->fcgLBRdoqLevel->Text = L"rdoq-level";
            // 
            // fcgCBDeblock
            // 
            this->fcgCBDeblock->AutoSize = true;
            this->fcgCBDeblock->Location = System::Drawing::Point(458, 411);
            this->fcgCBDeblock->Name = L"fcgCBDeblock";
            this->fcgCBDeblock->Size = System::Drawing::Size(141, 18);
            this->fcgCBDeblock->TabIndex = 61;
            this->fcgCBDeblock->Tag = L"reCmd";
            this->fcgCBDeblock->Text = L"インループ デブロックフィルタ";
            this->fcgCBDeblock->UseVisualStyleBackColor = true;
            // 
            // fcggroupBoxDeblock
            // 
            this->fcggroupBoxDeblock->Controls->Add(this->fcgNUDeblockThreshold);
            this->fcggroupBoxDeblock->Controls->Add(this->fcgNUDeblockStrength);
            this->fcggroupBoxDeblock->Controls->Add(this->fcgLBDeblockThreshold);
            this->fcggroupBoxDeblock->Controls->Add(this->fcgLBDeblockStrength);
            this->fcggroupBoxDeblock->Location = System::Drawing::Point(453, 413);
            this->fcggroupBoxDeblock->Name = L"fcggroupBoxDeblock";
            this->fcggroupBoxDeblock->Size = System::Drawing::Size(150, 75);
            this->fcggroupBoxDeblock->TabIndex = 62;
            this->fcggroupBoxDeblock->TabStop = false;
            // 
            // fcgNUDeblockThreshold
            // 
            this->fcgNUDeblockThreshold->Location = System::Drawing::Point(90, 47);
            this->fcgNUDeblockThreshold->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 6, 0, 0, 0 });
            this->fcgNUDeblockThreshold->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 6, 0, 0, System::Int32::MinValue });
            this->fcgNUDeblockThreshold->Name = L"fcgNUDeblockThreshold";
            this->fcgNUDeblockThreshold->Size = System::Drawing::Size(54, 21);
            this->fcgNUDeblockThreshold->TabIndex = 64;
            this->fcgNUDeblockThreshold->Tag = L"reCmd";
            this->fcgNUDeblockThreshold->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgNUDeblockStrength
            // 
            this->fcgNUDeblockStrength->Location = System::Drawing::Point(90, 20);
            this->fcgNUDeblockStrength->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 6, 0, 0, 0 });
            this->fcgNUDeblockStrength->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 6, 0, 0, System::Int32::MinValue });
            this->fcgNUDeblockStrength->Name = L"fcgNUDeblockStrength";
            this->fcgNUDeblockStrength->Size = System::Drawing::Size(54, 21);
            this->fcgNUDeblockStrength->TabIndex = 63;
            this->fcgNUDeblockStrength->Tag = L"reCmd";
            this->fcgNUDeblockStrength->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBDeblockThreshold
            // 
            this->fcgLBDeblockThreshold->AutoSize = true;
            this->fcgLBDeblockThreshold->Location = System::Drawing::Point(6, 49);
            this->fcgLBDeblockThreshold->Name = L"fcgLBDeblockThreshold";
            this->fcgLBDeblockThreshold->Size = System::Drawing::Size(68, 14);
            this->fcgLBDeblockThreshold->TabIndex = 1;
            this->fcgLBDeblockThreshold->Text = L"デブロック閾値";
            // 
            // fcgLBDeblockStrength
            // 
            this->fcgLBDeblockStrength->AutoSize = true;
            this->fcgLBDeblockStrength->Location = System::Drawing::Point(6, 22);
            this->fcgLBDeblockStrength->Name = L"fcgLBDeblockStrength";
            this->fcgLBDeblockStrength->Size = System::Drawing::Size(68, 14);
            this->fcgLBDeblockStrength->TabIndex = 0;
            this->fcgLBDeblockStrength->Text = L"デブロック強度";
            // 
            // fcgNUPsyRDOQ
            // 
            this->fcgNUPsyRDOQ->DecimalPlaces = 2;
            this->fcgNUPsyRDOQ->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 131072 });
            this->fcgNUPsyRDOQ->Location = System::Drawing::Point(383, 380);
            this->fcgNUPsyRDOQ->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 50, 0, 0, 0 });
            this->fcgNUPsyRDOQ->Name = L"fcgNUPsyRDOQ";
            this->fcgNUPsyRDOQ->Size = System::Drawing::Size(59, 21);
            this->fcgNUPsyRDOQ->TabIndex = 51;
            this->fcgNUPsyRDOQ->Tag = L"reCmd";
            this->fcgNUPsyRDOQ->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBPsyRDOQ
            // 
            this->fcgLBPsyRDOQ->AutoSize = true;
            this->fcgLBPsyRDOQ->Location = System::Drawing::Point(255, 382);
            this->fcgLBPsyRDOQ->Name = L"fcgLBPsyRDOQ";
            this->fcgLBPsyRDOQ->Size = System::Drawing::Size(56, 14);
            this->fcgLBPsyRDOQ->TabIndex = 105;
            this->fcgLBPsyRDOQ->Text = L"psy-rdoq";
            // 
            // fcgNUPsyRD
            // 
            this->fcgNUPsyRD->DecimalPlaces = 2;
            this->fcgNUPsyRD->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 131072 });
            this->fcgNUPsyRD->Location = System::Drawing::Point(383, 352);
            this->fcgNUPsyRD->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 0 });
            this->fcgNUPsyRD->Name = L"fcgNUPsyRD";
            this->fcgNUPsyRD->Size = System::Drawing::Size(59, 21);
            this->fcgNUPsyRD->TabIndex = 50;
            this->fcgNUPsyRD->Tag = L"reCmd";
            this->fcgNUPsyRD->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBPsyRD
            // 
            this->fcgLBPsyRD->AutoSize = true;
            this->fcgLBPsyRD->Location = System::Drawing::Point(255, 354);
            this->fcgLBPsyRD->Name = L"fcgLBPsyRD";
            this->fcgLBPsyRD->Size = System::Drawing::Size(42, 14);
            this->fcgLBPsyRD->TabIndex = 103;
            this->fcgLBPsyRD->Text = L"psy-rd";
            // 
            // fcggroupBoxFrame
            // 
            this->fcggroupBoxFrame->Controls->Add(this->fcgNUHistThreshold);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBFade);
            this->fcggroupBoxFrame->Controls->Add(this->fcgCBFade);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBSceneCutMode);
            this->fcggroupBoxFrame->Controls->Add(this->fcgCXSceneCutMode);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBWeightB);
            this->fcggroupBoxFrame->Controls->Add(this->fcgCBWeightB);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBScenecut);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBRef);
            this->fcggroupBoxFrame->Controls->Add(this->fcgNURef);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBBframes);
            this->fcggroupBoxFrame->Controls->Add(this->fcgNUBframes);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBBadapt);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBKeyintMax);
            this->fcggroupBoxFrame->Controls->Add(this->fcgNUKeyintMax);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBRcLookAhead);
            this->fcggroupBoxFrame->Controls->Add(this->fcgCBWeightP);
            this->fcggroupBoxFrame->Controls->Add(this->fcgNURCLookahead);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBWeightP);
            this->fcggroupBoxFrame->Controls->Add(this->fcgCXBadapt);
            this->fcggroupBoxFrame->Controls->Add(this->fcgNUScenecut);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBBpyramid);
            this->fcggroupBoxFrame->Controls->Add(this->fcgCBBpyramid);
            this->fcggroupBoxFrame->Controls->Add(this->fcgNUKeyintMin);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBOpenGOP);
            this->fcggroupBoxFrame->Controls->Add(this->fcgLBKeyintMin);
            this->fcggroupBoxFrame->Controls->Add(this->fcgCBOpenGOP);
            this->fcggroupBoxFrame->Location = System::Drawing::Point(368, 3);
            this->fcggroupBoxFrame->Name = L"fcggroupBoxFrame";
            this->fcggroupBoxFrame->Size = System::Drawing::Size(230, 338);
            this->fcggroupBoxFrame->TabIndex = 20;
            this->fcggroupBoxFrame->TabStop = false;
            // 
            // fcgNUHistThreshold
            // 
            this->fcgNUHistThreshold->DecimalPlaces = 3;
            this->fcgNUHistThreshold->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 196608 });
            this->fcgNUHistThreshold->Location = System::Drawing::Point(146, 41);
            this->fcgNUHistThreshold->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
            this->fcgNUHistThreshold->Name = L"fcgNUHistThreshold";
            this->fcgNUHistThreshold->Size = System::Drawing::Size(65, 21);
            this->fcgNUHistThreshold->TabIndex = 99;
            this->fcgNUHistThreshold->Tag = L"reCmd";
            this->fcgNUHistThreshold->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBFade
            // 
            this->fcgLBFade->AutoSize = true;
            this->fcgLBFade->Location = System::Drawing::Point(17, 68);
            this->fcgLBFade->Name = L"fcgLBFade";
            this->fcgLBFade->Size = System::Drawing::Size(60, 14);
            this->fcgLBFade->TabIndex = 98;
            this->fcgLBFade->Text = L"フェード検出";
            // 
            // fcgCBFade
            // 
            this->fcgCBFade->AutoSize = true;
            this->fcgCBFade->Location = System::Drawing::Point(188, 69);
            this->fcgCBFade->Name = L"fcgCBFade";
            this->fcgCBFade->Size = System::Drawing::Size(15, 14);
            this->fcgCBFade->TabIndex = 97;
            this->fcgCBFade->Tag = L"reCmd";
            this->fcgCBFade->UseVisualStyleBackColor = true;
            // 
            // fcgLBSceneCutMode
            // 
            this->fcgLBSceneCutMode->AutoSize = true;
            this->fcgLBSceneCutMode->Location = System::Drawing::Point(18, 16);
            this->fcgLBSceneCutMode->Name = L"fcgLBSceneCutMode";
            this->fcgLBSceneCutMode->Size = System::Drawing::Size(55, 14);
            this->fcgLBSceneCutMode->TabIndex = 96;
            this->fcgLBSceneCutMode->Text = L"シーンカット";
            // 
            // fcgCXSceneCutMode
            // 
            this->fcgCXSceneCutMode->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXSceneCutMode->FormattingEnabled = true;
            this->fcgCXSceneCutMode->Location = System::Drawing::Point(126, 13);
            this->fcgCXSceneCutMode->Name = L"fcgCXSceneCutMode";
            this->fcgCXSceneCutMode->Size = System::Drawing::Size(88, 22);
            this->fcgCXSceneCutMode->TabIndex = 95;
            this->fcgCXSceneCutMode->Tag = L"reCmd";
            this->fcgCXSceneCutMode->SelectedIndexChanged += gcnew System::EventHandler(this, &frmConfig::fcgCXSceneCutMode_SelectedIndexChanged);
            // 
            // fcgLBWeightB
            // 
            this->fcgLBWeightB->AutoSize = true;
            this->fcgLBWeightB->Location = System::Drawing::Point(17, 293);
            this->fcgLBWeightB->Name = L"fcgLBWeightB";
            this->fcgLBWeightB->Size = System::Drawing::Size(87, 14);
            this->fcgLBWeightB->TabIndex = 94;
            this->fcgLBWeightB->Text = L"重み付きBフレーム";
            // 
            // fcgCBWeightB
            // 
            this->fcgCBWeightB->AutoSize = true;
            this->fcgCBWeightB->Location = System::Drawing::Point(188, 294);
            this->fcgCBWeightB->Name = L"fcgCBWeightB";
            this->fcgCBWeightB->Size = System::Drawing::Size(15, 14);
            this->fcgCBWeightB->TabIndex = 30;
            this->fcgCBWeightB->Tag = L"reCmd";
            this->fcgCBWeightB->UseVisualStyleBackColor = true;
            // 
            // fcgLBScenecut
            // 
            this->fcgLBScenecut->AutoSize = true;
            this->fcgLBScenecut->Location = System::Drawing::Point(17, 43);
            this->fcgLBScenecut->Name = L"fcgLBScenecut";
            this->fcgLBScenecut->Size = System::Drawing::Size(77, 14);
            this->fcgLBScenecut->TabIndex = 90;
            this->fcgLBScenecut->Text = L"シーンカット閾値";
            // 
            // fcgLBRef
            // 
            this->fcgLBRef->AutoSize = true;
            this->fcgLBRef->Location = System::Drawing::Point(17, 192);
            this->fcgLBRef->Name = L"fcgLBRef";
            this->fcgLBRef->Size = System::Drawing::Size(51, 14);
            this->fcgLBRef->TabIndex = 78;
            this->fcgLBRef->Text = L"参照距離";
            // 
            // fcgNURef
            // 
            this->fcgNURef->Location = System::Drawing::Point(146, 190);
            this->fcgNURef->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 16, 0, 0, 0 });
            this->fcgNURef->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            this->fcgNURef->Name = L"fcgNURef";
            this->fcgNURef->Size = System::Drawing::Size(65, 21);
            this->fcgNURef->TabIndex = 26;
            this->fcgNURef->Tag = L"reCmd";
            this->fcgNURef->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNURef->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            // 
            // fcgLBBframes
            // 
            this->fcgLBBframes->AutoSize = true;
            this->fcgLBBframes->Location = System::Drawing::Point(17, 218);
            this->fcgLBBframes->Name = L"fcgLBBframes";
            this->fcgLBBframes->Size = System::Drawing::Size(102, 14);
            this->fcgLBBframes->TabIndex = 80;
            this->fcgLBBframes->Text = L"最大連続Bフレーム数";
            // 
            // fcgNUBframes
            // 
            this->fcgNUBframes->Location = System::Drawing::Point(146, 216);
            this->fcgNUBframes->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 16, 0, 0, 0 });
            this->fcgNUBframes->Name = L"fcgNUBframes";
            this->fcgNUBframes->Size = System::Drawing::Size(65, 21);
            this->fcgNUBframes->TabIndex = 27;
            this->fcgNUBframes->Tag = L"reCmd";
            this->fcgNUBframes->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBBadapt
            // 
            this->fcgLBBadapt->AutoSize = true;
            this->fcgLBBadapt->Location = System::Drawing::Point(17, 245);
            this->fcgLBBadapt->Name = L"fcgLBBadapt";
            this->fcgLBBadapt->Size = System::Drawing::Size(102, 14);
            this->fcgLBBadapt->TabIndex = 82;
            this->fcgLBBadapt->Text = L"適応的Bフレーム挿入";
            // 
            // fcgLBKeyintMax
            // 
            this->fcgLBKeyintMax->AutoSize = true;
            this->fcgLBKeyintMax->Location = System::Drawing::Point(17, 117);
            this->fcgLBKeyintMax->Name = L"fcgLBKeyintMax";
            this->fcgLBKeyintMax->Size = System::Drawing::Size(102, 14);
            this->fcgLBKeyintMax->TabIndex = 74;
            this->fcgLBKeyintMax->Text = L"最大キーフレーム間隔";
            // 
            // fcgNUKeyintMax
            // 
            this->fcgNUKeyintMax->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
            this->fcgNUKeyintMax->Location = System::Drawing::Point(146, 115);
            this->fcgNUKeyintMax->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3000, 0, 0, 0 });
            this->fcgNUKeyintMax->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, System::Int32::MinValue });
            this->fcgNUKeyintMax->Name = L"fcgNUKeyintMax";
            this->fcgNUKeyintMax->Size = System::Drawing::Size(65, 21);
            this->fcgNUKeyintMax->TabIndex = 23;
            this->fcgNUKeyintMax->Tag = L"reCmd";
            this->fcgNUKeyintMax->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBRcLookAhead
            // 
            this->fcgLBRcLookAhead->AutoSize = true;
            this->fcgLBRcLookAhead->Location = System::Drawing::Point(17, 166);
            this->fcgLBRcLookAhead->Name = L"fcgLBRcLookAhead";
            this->fcgLBRcLookAhead->Size = System::Drawing::Size(95, 14);
            this->fcgLBRcLookAhead->TabIndex = 76;
            this->fcgLBRcLookAhead->Text = L"先行探索フレーム数";
            // 
            // fcgCBWeightP
            // 
            this->fcgCBWeightP->AutoSize = true;
            this->fcgCBWeightP->Location = System::Drawing::Point(188, 317);
            this->fcgCBWeightP->Name = L"fcgCBWeightP";
            this->fcgCBWeightP->Size = System::Drawing::Size(15, 14);
            this->fcgCBWeightP->TabIndex = 31;
            this->fcgCBWeightP->Tag = L"reCmd";
            this->fcgCBWeightP->UseVisualStyleBackColor = true;
            // 
            // fcgNURCLookahead
            // 
            this->fcgNURCLookahead->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
            this->fcgNURCLookahead->Location = System::Drawing::Point(146, 164);
            this->fcgNURCLookahead->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 250, 0, 0, 0 });
            this->fcgNURCLookahead->Name = L"fcgNURCLookahead";
            this->fcgNURCLookahead->Size = System::Drawing::Size(65, 21);
            this->fcgNURCLookahead->TabIndex = 25;
            this->fcgNURCLookahead->Tag = L"reCmd";
            this->fcgNURCLookahead->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBWeightP
            // 
            this->fcgLBWeightP->AutoSize = true;
            this->fcgLBWeightP->Location = System::Drawing::Point(17, 316);
            this->fcgLBWeightP->Name = L"fcgLBWeightP";
            this->fcgLBWeightP->Size = System::Drawing::Size(87, 14);
            this->fcgLBWeightP->TabIndex = 92;
            this->fcgLBWeightP->Text = L"重み付きPフレーム";
            // 
            // fcgCXBadapt
            // 
            this->fcgCXBadapt->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXBadapt->FormattingEnabled = true;
            this->fcgCXBadapt->Location = System::Drawing::Point(125, 242);
            this->fcgCXBadapt->Name = L"fcgCXBadapt";
            this->fcgCXBadapt->Size = System::Drawing::Size(88, 22);
            this->fcgCXBadapt->TabIndex = 28;
            this->fcgCXBadapt->Tag = L"reCmd";
            // 
            // fcgNUScenecut
            // 
            this->fcgNUScenecut->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
            this->fcgNUScenecut->Location = System::Drawing::Point(146, 41);
            this->fcgNUScenecut->Name = L"fcgNUScenecut";
            this->fcgNUScenecut->Size = System::Drawing::Size(65, 21);
            this->fcgNUScenecut->TabIndex = 21;
            this->fcgNUScenecut->Tag = L"reCmd";
            this->fcgNUScenecut->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBBpyramid
            // 
            this->fcgLBBpyramid->AutoSize = true;
            this->fcgLBBpyramid->Location = System::Drawing::Point(17, 270);
            this->fcgLBBpyramid->Name = L"fcgLBBpyramid";
            this->fcgLBBpyramid->Size = System::Drawing::Size(66, 14);
            this->fcgLBBpyramid->TabIndex = 84;
            this->fcgLBBpyramid->Text = L"ピラミッド参照";
            // 
            // fcgCBBpyramid
            // 
            this->fcgCBBpyramid->AutoSize = true;
            this->fcgCBBpyramid->Location = System::Drawing::Point(188, 271);
            this->fcgCBBpyramid->Name = L"fcgCBBpyramid";
            this->fcgCBBpyramid->Size = System::Drawing::Size(15, 14);
            this->fcgCBBpyramid->TabIndex = 29;
            this->fcgCBBpyramid->Tag = L"reCmd";
            this->fcgCBBpyramid->UseVisualStyleBackColor = true;
            // 
            // fcgNUKeyintMin
            // 
            this->fcgNUKeyintMin->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
            this->fcgNUKeyintMin->Location = System::Drawing::Point(146, 89);
            this->fcgNUKeyintMin->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3000, 0, 0, 0 });
            this->fcgNUKeyintMin->Name = L"fcgNUKeyintMin";
            this->fcgNUKeyintMin->Size = System::Drawing::Size(65, 21);
            this->fcgNUKeyintMin->TabIndex = 22;
            this->fcgNUKeyintMin->Tag = L"reCmd";
            this->fcgNUKeyintMin->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBOpenGOP
            // 
            this->fcgLBOpenGOP->AutoSize = true;
            this->fcgLBOpenGOP->Location = System::Drawing::Point(17, 142);
            this->fcgLBOpenGOP->Name = L"fcgLBOpenGOP";
            this->fcgLBOpenGOP->Size = System::Drawing::Size(62, 14);
            this->fcgLBOpenGOP->TabIndex = 86;
            this->fcgLBOpenGOP->Text = L"Open GOP";
            // 
            // fcgLBKeyintMin
            // 
            this->fcgLBKeyintMin->AutoSize = true;
            this->fcgLBKeyintMin->Location = System::Drawing::Point(17, 91);
            this->fcgLBKeyintMin->Name = L"fcgLBKeyintMin";
            this->fcgLBKeyintMin->Size = System::Drawing::Size(102, 14);
            this->fcgLBKeyintMin->TabIndex = 88;
            this->fcgLBKeyintMin->Text = L"最小キーフレーム間隔";
            // 
            // fcgCBOpenGOP
            // 
            this->fcgCBOpenGOP->AutoSize = true;
            this->fcgCBOpenGOP->Location = System::Drawing::Point(188, 143);
            this->fcgCBOpenGOP->Name = L"fcgCBOpenGOP";
            this->fcgCBOpenGOP->Size = System::Drawing::Size(15, 14);
            this->fcgCBOpenGOP->TabIndex = 24;
            this->fcgCBOpenGOP->Tag = L"reCmd";
            this->fcgCBOpenGOP->UseVisualStyleBackColor = true;
            // 
            // fcgCBCUTree
            // 
            this->fcgCBCUTree->AutoSize = true;
            this->fcgCBCUTree->Location = System::Drawing::Point(587, 382);
            this->fcgCBCUTree->Name = L"fcgCBCUTree";
            this->fcgCBCUTree->Size = System::Drawing::Size(15, 14);
            this->fcgCBCUTree->TabIndex = 43;
            this->fcgCBCUTree->Tag = L"reCmd";
            this->fcgCBCUTree->UseVisualStyleBackColor = true;
            // 
            // fcgLBCUTree
            // 
            this->fcgLBCUTree->AutoSize = true;
            this->fcgLBCUTree->Location = System::Drawing::Point(460, 381);
            this->fcgLBCUTree->Name = L"fcgLBCUTree";
            this->fcgLBCUTree->Size = System::Drawing::Size(45, 14);
            this->fcgLBCUTree->TabIndex = 100;
            this->fcgLBCUTree->Text = L"CUTree";
            // 
            // fcgNURD
            // 
            this->fcgNURD->Location = System::Drawing::Point(397, 436);
            this->fcgNURD->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 6, 0, 0, 0 });
            this->fcgNURD->Name = L"fcgNURD";
            this->fcgNURD->Size = System::Drawing::Size(45, 21);
            this->fcgNURD->TabIndex = 53;
            this->fcgNURD->Tag = L"reCmd";
            this->fcgNURD->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgNUAQStrength
            // 
            this->fcgNUAQStrength->DecimalPlaces = 2;
            this->fcgNUAQStrength->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 131072 });
            this->fcgNUAQStrength->Location = System::Drawing::Point(102, 382);
            this->fcgNUAQStrength->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3, 0, 0, 0 });
            this->fcgNUAQStrength->Name = L"fcgNUAQStrength";
            this->fcgNUAQStrength->Size = System::Drawing::Size(59, 21);
            this->fcgNUAQStrength->TabIndex = 41;
            this->fcgNUAQStrength->Tag = L"reCmd";
            this->fcgNUAQStrength->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgLBAQStrength
            // 
            this->fcgLBAQStrength->AutoSize = true;
            this->fcgLBAQStrength->Location = System::Drawing::Point(6, 384);
            this->fcgLBAQStrength->Name = L"fcgLBAQStrength";
            this->fcgLBAQStrength->Size = System::Drawing::Size(41, 14);
            this->fcgLBAQStrength->TabIndex = 97;
            this->fcgLBAQStrength->Text = L"AQ強さ";
            // 
            // fcgCXAQMode
            // 
            this->fcgCXAQMode->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXAQMode->FormattingEnabled = true;
            this->fcgCXAQMode->Location = System::Drawing::Point(102, 353);
            this->fcgCXAQMode->Name = L"fcgCXAQMode";
            this->fcgCXAQMode->Size = System::Drawing::Size(142, 22);
            this->fcgCXAQMode->TabIndex = 40;
            this->fcgCXAQMode->Tag = L"reCmd";
            // 
            // fcgLBAQMode
            // 
            this->fcgLBAQMode->AutoSize = true;
            this->fcgLBAQMode->Location = System::Drawing::Point(6, 356);
            this->fcgLBAQMode->Name = L"fcgLBAQMode";
            this->fcgLBAQMode->Size = System::Drawing::Size(80, 14);
            this->fcgLBAQMode->TabIndex = 95;
            this->fcgLBAQMode->Text = L"適応的QP(AQ)";
            // 
            // fcgLBRD
            // 
            this->fcgLBRD->AutoSize = true;
            this->fcgLBRD->Location = System::Drawing::Point(255, 438);
            this->fcgLBRD->Name = L"fcgLBRD";
            this->fcgLBRD->Size = System::Drawing::Size(86, 14);
            this->fcgLBRD->TabIndex = 94;
            this->fcgLBRD->Text = L"レート歪み最適化";
            // 
            // fcgGroupBoxME
            // 
            this->fcgGroupBoxME->Controls->Add(this->fcgNURskipEdgeThreshold);
            this->fcgGroupBoxME->Controls->Add(this->fcgLBRskipEdgeThreshold);
            this->fcgGroupBoxME->Controls->Add(this->fcgNURskip);
            this->fcgGroupBoxME->Controls->Add(this->fcgLBNoRecusrionSkip);
            this->fcgGroupBoxME->Controls->Add(this->fcgLBME);
            this->fcgGroupBoxME->Controls->Add(this->fcgLBSubME);
            this->fcgGroupBoxME->Controls->Add(this->fcgNUMaxMerge);
            this->fcgGroupBoxME->Controls->Add(this->fcgLBMERange);
            this->fcgGroupBoxME->Controls->Add(this->fcgLBMaxMerge);
            this->fcgGroupBoxME->Controls->Add(this->fcgCXME);
            this->fcgGroupBoxME->Controls->Add(this->fcgNUMERange);
            this->fcgGroupBoxME->Controls->Add(this->fcgCXSubME);
            this->fcgGroupBoxME->Location = System::Drawing::Point(8, 195);
            this->fcgGroupBoxME->Name = L"fcgGroupBoxME";
            this->fcgGroupBoxME->Size = System::Drawing::Size(345, 146);
            this->fcgGroupBoxME->TabIndex = 10;
            this->fcgGroupBoxME->TabStop = false;
            // 
            // fcgNURskipEdgeThreshold
            // 
            this->fcgNURskipEdgeThreshold->Location = System::Drawing::Point(270, 120);
            this->fcgNURskipEdgeThreshold->Name = L"fcgNURskipEdgeThreshold";
            this->fcgNURskipEdgeThreshold->Size = System::Drawing::Size(65, 21);
            this->fcgNURskipEdgeThreshold->TabIndex = 85;
            this->fcgNURskipEdgeThreshold->Tag = L"reCmd";
            this->fcgNURskipEdgeThreshold->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNURskipEdgeThreshold->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            // 
            // fcgLBRskipEdgeThreshold
            // 
            this->fcgLBRskipEdgeThreshold->AutoSize = true;
            this->fcgLBRskipEdgeThreshold->Location = System::Drawing::Point(212, 123);
            this->fcgLBRskipEdgeThreshold->Name = L"fcgLBRskipEdgeThreshold";
            this->fcgLBRskipEdgeThreshold->Size = System::Drawing::Size(52, 14);
            this->fcgLBRskipEdgeThreshold->TabIndex = 84;
            this->fcgLBRskipEdgeThreshold->Text = L"エッジ閾値";
            // 
            // fcgNURskip
            // 
            this->fcgNURskip->Location = System::Drawing::Point(140, 120);
            this->fcgNURskip->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
            this->fcgNURskip->Name = L"fcgNURskip";
            this->fcgNURskip->Size = System::Drawing::Size(65, 21);
            this->fcgNURskip->TabIndex = 83;
            this->fcgNURskip->Tag = L"reCmd";
            this->fcgNURskip->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNURskip->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            // 
            // fcgLBNoRecusrionSkip
            // 
            this->fcgLBNoRecusrionSkip->AutoSize = true;
            this->fcgLBNoRecusrionSkip->Location = System::Drawing::Point(14, 122);
            this->fcgLBNoRecusrionSkip->Name = L"fcgLBNoRecusrionSkip";
            this->fcgLBNoRecusrionSkip->Size = System::Drawing::Size(73, 14);
            this->fcgLBNoRecusrionSkip->TabIndex = 82;
            this->fcgLBNoRecusrionSkip->Text = L"再帰分析制限";
            // 
            // fcgLBME
            // 
            this->fcgLBME->AutoSize = true;
            this->fcgLBME->Location = System::Drawing::Point(14, 18);
            this->fcgLBME->Name = L"fcgLBME";
            this->fcgLBME->Size = System::Drawing::Size(97, 14);
            this->fcgLBME->TabIndex = 57;
            this->fcgLBME->Text = L"動き予測アルゴリズム";
            // 
            // fcgLBSubME
            // 
            this->fcgLBSubME->AutoSize = true;
            this->fcgLBSubME->Location = System::Drawing::Point(14, 45);
            this->fcgLBSubME->Name = L"fcgLBSubME";
            this->fcgLBSubME->Size = System::Drawing::Size(99, 14);
            this->fcgLBSubME->TabIndex = 59;
            this->fcgLBSubME->Text = L"サブピクセル動き予測";
            // 
            // fcgNUMaxMerge
            // 
            this->fcgNUMaxMerge->Location = System::Drawing::Point(140, 96);
            this->fcgNUMaxMerge->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 0 });
            this->fcgNUMaxMerge->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            this->fcgNUMaxMerge->Name = L"fcgNUMaxMerge";
            this->fcgNUMaxMerge->Size = System::Drawing::Size(65, 21);
            this->fcgNUMaxMerge->TabIndex = 14;
            this->fcgNUMaxMerge->Tag = L"reCmd";
            this->fcgNUMaxMerge->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNUMaxMerge->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            // 
            // fcgLBMERange
            // 
            this->fcgLBMERange->AutoSize = true;
            this->fcgLBMERange->Location = System::Drawing::Point(14, 72);
            this->fcgLBMERange->Name = L"fcgLBMERange";
            this->fcgLBMERange->Size = System::Drawing::Size(70, 14);
            this->fcgLBMERange->TabIndex = 61;
            this->fcgLBMERange->Text = L"動き探索範囲";
            // 
            // fcgLBMaxMerge
            // 
            this->fcgLBMaxMerge->AutoSize = true;
            this->fcgLBMaxMerge->Location = System::Drawing::Point(14, 98);
            this->fcgLBMaxMerge->Name = L"fcgLBMaxMerge";
            this->fcgLBMaxMerge->Size = System::Drawing::Size(103, 14);
            this->fcgLBMaxMerge->TabIndex = 63;
            this->fcgLBMaxMerge->Text = L"最大動き領域統合数";
            // 
            // fcgCXME
            // 
            this->fcgCXME->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXME->FormattingEnabled = true;
            this->fcgCXME->Location = System::Drawing::Point(140, 15);
            this->fcgCXME->Name = L"fcgCXME";
            this->fcgCXME->Size = System::Drawing::Size(195, 22);
            this->fcgCXME->TabIndex = 11;
            this->fcgCXME->Tag = L"reCmd";
            // 
            // fcgNUMERange
            // 
            this->fcgNUMERange->Location = System::Drawing::Point(140, 70);
            this->fcgNUMERange->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 32768, 0, 0, 0 });
            this->fcgNUMERange->Name = L"fcgNUMERange";
            this->fcgNUMERange->Size = System::Drawing::Size(65, 21);
            this->fcgNUMERange->TabIndex = 13;
            this->fcgNUMERange->Tag = L"reCmd";
            this->fcgNUMERange->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgCXSubME
            // 
            this->fcgCXSubME->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXSubME->FormattingEnabled = true;
            this->fcgCXSubME->Location = System::Drawing::Point(140, 42);
            this->fcgCXSubME->Name = L"fcgCXSubME";
            this->fcgCXSubME->Size = System::Drawing::Size(195, 22);
            this->fcgCXSubME->TabIndex = 12;
            this->fcgCXSubME->Tag = L"reCmd";
            // 
            // fcgGroupBoxCTU
            // 
            this->fcgGroupBoxCTU->Controls->Add(this->fcgLBLimitModes);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgCBLimitModes);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgLBLimitRefs);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgNULimitRefs);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgLBAsymmetricMP);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgCBAsymmetricMP);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgLBRectMP);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgCBRectMP);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgLBCtu);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgLBTuInterDepth);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgNUCtu);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgLBTuIntraDepth);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgNUTuInterDepth);
            this->fcgGroupBoxCTU->Controls->Add(this->fcgNUTuIntraDepth);
            this->fcgGroupBoxCTU->Location = System::Drawing::Point(8, 3);
            this->fcgGroupBoxCTU->Name = L"fcgGroupBoxCTU";
            this->fcgGroupBoxCTU->Size = System::Drawing::Size(345, 188);
            this->fcgGroupBoxCTU->TabIndex = 0;
            this->fcgGroupBoxCTU->TabStop = false;
            // 
            // fcgLBLimitModes
            // 
            this->fcgLBLimitModes->AutoSize = true;
            this->fcgLBLimitModes->Location = System::Drawing::Point(16, 164);
            this->fcgLBLimitModes->Name = L"fcgLBLimitModes";
            this->fcgLBLimitModes->Size = System::Drawing::Size(76, 14);
            this->fcgLBLimitModes->TabIndex = 82;
            this->fcgLBLimitModes->Text = L"モード解析制限";
            // 
            // fcgCBLimitModes
            // 
            this->fcgCBLimitModes->AutoSize = true;
            this->fcgCBLimitModes->Location = System::Drawing::Point(181, 165);
            this->fcgCBLimitModes->Name = L"fcgCBLimitModes";
            this->fcgCBLimitModes->Size = System::Drawing::Size(15, 14);
            this->fcgCBLimitModes->TabIndex = 7;
            this->fcgCBLimitModes->Tag = L"reCmd";
            this->fcgCBLimitModes->UseVisualStyleBackColor = true;
            // 
            // fcgLBLimitRefs
            // 
            this->fcgLBLimitRefs->AutoSize = true;
            this->fcgLBLimitRefs->Location = System::Drawing::Point(16, 139);
            this->fcgLBLimitRefs->Name = L"fcgLBLimitRefs";
            this->fcgLBLimitRefs->Size = System::Drawing::Size(73, 14);
            this->fcgLBLimitRefs->TabIndex = 80;
            this->fcgLBLimitRefs->Text = L"参照解析制限";
            // 
            // fcgNULimitRefs
            // 
            this->fcgNULimitRefs->Location = System::Drawing::Point(143, 137);
            this->fcgNULimitRefs->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3, 0, 0, 0 });
            this->fcgNULimitRefs->Name = L"fcgNULimitRefs";
            this->fcgNULimitRefs->Size = System::Drawing::Size(64, 21);
            this->fcgNULimitRefs->TabIndex = 6;
            this->fcgNULimitRefs->Tag = L"reCmd";
            this->fcgNULimitRefs->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNULimitRefs->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            // 
            // fcgLBAsymmetricMP
            // 
            this->fcgLBAsymmetricMP->AutoSize = true;
            this->fcgLBAsymmetricMP->Location = System::Drawing::Point(16, 115);
            this->fcgLBAsymmetricMP->Name = L"fcgLBAsymmetricMP";
            this->fcgLBAsymmetricMP->Size = System::Drawing::Size(92, 14);
            this->fcgLBAsymmetricMP->TabIndex = 78;
            this->fcgLBAsymmetricMP->Text = L"非対称予測ブロック";
            // 
            // fcgCBAsymmetricMP
            // 
            this->fcgCBAsymmetricMP->AutoSize = true;
            this->fcgCBAsymmetricMP->Location = System::Drawing::Point(181, 116);
            this->fcgCBAsymmetricMP->Name = L"fcgCBAsymmetricMP";
            this->fcgCBAsymmetricMP->Size = System::Drawing::Size(15, 14);
            this->fcgCBAsymmetricMP->TabIndex = 5;
            this->fcgCBAsymmetricMP->Tag = L"reCmd";
            this->fcgCBAsymmetricMP->UseVisualStyleBackColor = true;
            // 
            // fcgLBRectMP
            // 
            this->fcgLBRectMP->AutoSize = true;
            this->fcgLBRectMP->Location = System::Drawing::Point(16, 91);
            this->fcgLBRectMP->Name = L"fcgLBRectMP";
            this->fcgLBRectMP->Size = System::Drawing::Size(92, 14);
            this->fcgLBRectMP->TabIndex = 77;
            this->fcgLBRectMP->Text = L"非正方予測ブロック";
            // 
            // fcgCBRectMP
            // 
            this->fcgCBRectMP->AutoSize = true;
            this->fcgCBRectMP->Location = System::Drawing::Point(181, 92);
            this->fcgCBRectMP->Name = L"fcgCBRectMP";
            this->fcgCBRectMP->Size = System::Drawing::Size(15, 14);
            this->fcgCBRectMP->TabIndex = 4;
            this->fcgCBRectMP->Tag = L"reCmd";
            this->fcgCBRectMP->UseVisualStyleBackColor = true;
            // 
            // fcgLBCtu
            // 
            this->fcgLBCtu->AutoSize = true;
            this->fcgLBCtu->Location = System::Drawing::Point(14, 15);
            this->fcgLBCtu->Name = L"fcgLBCtu";
            this->fcgLBCtu->Size = System::Drawing::Size(96, 14);
            this->fcgLBCtu->TabIndex = 65;
            this->fcgLBCtu->Text = L"符号化ブロックサイズ";
            // 
            // fcgLBTuInterDepth
            // 
            this->fcgLBTuInterDepth->AutoSize = true;
            this->fcgLBTuInterDepth->Location = System::Drawing::Point(14, 65);
            this->fcgLBTuInterDepth->Name = L"fcgLBTuInterDepth";
            this->fcgLBTuInterDepth->Size = System::Drawing::Size(114, 14);
            this->fcgLBTuInterDepth->TabIndex = 69;
            this->fcgLBTuInterDepth->Text = L"インター予測ブロック階層";
            // 
            // fcgNUCtu
            // 
            this->fcgNUCtu->Location = System::Drawing::Point(143, 13);
            this->fcgNUCtu->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 64, 0, 0, 0 });
            this->fcgNUCtu->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 16, 0, 0, 0 });
            this->fcgNUCtu->Name = L"fcgNUCtu";
            this->fcgNUCtu->Size = System::Drawing::Size(64, 21);
            this->fcgNUCtu->TabIndex = 1;
            this->fcgNUCtu->Tag = L"reCmd";
            this->fcgNUCtu->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNUCtu->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 16, 0, 0, 0 });
            this->fcgNUCtu->ValueChanged += gcnew System::EventHandler(this, &frmConfig::fcgNUCtu_ValueChanged);
            // 
            // fcgLBTuIntraDepth
            // 
            this->fcgLBTuIntraDepth->AutoSize = true;
            this->fcgLBTuIntraDepth->Location = System::Drawing::Point(14, 40);
            this->fcgLBTuIntraDepth->Name = L"fcgLBTuIntraDepth";
            this->fcgLBTuIntraDepth->Size = System::Drawing::Size(113, 14);
            this->fcgLBTuIntraDepth->TabIndex = 67;
            this->fcgLBTuIntraDepth->Text = L"イントラ予測ブロック階層";
            // 
            // fcgNUTuInterDepth
            // 
            this->fcgNUTuInterDepth->Location = System::Drawing::Point(143, 63);
            this->fcgNUTuInterDepth->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->fcgNUTuInterDepth->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            this->fcgNUTuInterDepth->Name = L"fcgNUTuInterDepth";
            this->fcgNUTuInterDepth->Size = System::Drawing::Size(64, 21);
            this->fcgNUTuInterDepth->TabIndex = 3;
            this->fcgNUTuInterDepth->Tag = L"reCmd";
            this->fcgNUTuInterDepth->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNUTuInterDepth->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            // 
            // fcgNUTuIntraDepth
            // 
            this->fcgNUTuIntraDepth->Location = System::Drawing::Point(143, 38);
            this->fcgNUTuIntraDepth->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->fcgNUTuIntraDepth->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            this->fcgNUTuIntraDepth->Name = L"fcgNUTuIntraDepth";
            this->fcgNUTuIntraDepth->Size = System::Drawing::Size(64, 21);
            this->fcgNUTuIntraDepth->TabIndex = 2;
            this->fcgNUTuIntraDepth->Tag = L"reCmd";
            this->fcgNUTuIntraDepth->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNUTuIntraDepth->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            // 
            // fcgCBSAO
            // 
            this->fcgCBSAO->AutoSize = true;
            this->fcgCBSAO->Location = System::Drawing::Point(587, 357);
            this->fcgCBSAO->Name = L"fcgCBSAO";
            this->fcgCBSAO->Size = System::Drawing::Size(15, 14);
            this->fcgCBSAO->TabIndex = 60;
            this->fcgCBSAO->Tag = L"reCmd";
            this->fcgCBSAO->UseVisualStyleBackColor = true;
            // 
            // fcgLBSAO
            // 
            this->fcgLBSAO->AutoSize = true;
            this->fcgLBSAO->Location = System::Drawing::Point(459, 355);
            this->fcgLBSAO->Name = L"fcgLBSAO";
            this->fcgLBSAO->Size = System::Drawing::Size(121, 14);
            this->fcgLBSAO->TabIndex = 39;
            this->fcgLBSAO->Text = L"画素適応オフセット(SAO)";
            // 
            // fcgtabPageX2652pass
            // 
            this->fcgtabPageX2652pass->Controls->Add(this->fcgCBSVT);
            this->fcgtabPageX2652pass->Controls->Add(this->fcgCBAnalysisReuse);
            this->fcgtabPageX2652pass->Controls->Add(this->fcggroupBoxAnalysisReuse);
            this->fcgtabPageX2652pass->Location = System::Drawing::Point(4, 23);
            this->fcgtabPageX2652pass->Name = L"fcgtabPageX2652pass";
            this->fcgtabPageX2652pass->Size = System::Drawing::Size(608, 493);
            this->fcgtabPageX2652pass->TabIndex = 7;
            this->fcgtabPageX2652pass->Text = L"x265 (3)";
            this->fcgtabPageX2652pass->UseVisualStyleBackColor = true;
            // 
            // fcgCBSVT
            // 
            this->fcgCBSVT->AutoSize = true;
            this->fcgCBSVT->Location = System::Drawing::Point(17, 14);
            this->fcgCBSVT->Name = L"fcgCBSVT";
            this->fcgCBSVT->Size = System::Drawing::Size(73, 18);
            this->fcgCBSVT->TabIndex = 101;
            this->fcgCBSVT->Tag = L"reCmd";
            this->fcgCBSVT->Text = L"SVTモード";
            this->fcgCBSVT->UseVisualStyleBackColor = true;
            // 
            // fcgCBAnalysisReuse
            // 
            this->fcgCBAnalysisReuse->AutoSize = true;
            this->fcgCBAnalysisReuse->Location = System::Drawing::Point(17, 38);
            this->fcgCBAnalysisReuse->Name = L"fcgCBAnalysisReuse";
            this->fcgCBAnalysisReuse->Size = System::Drawing::Size(112, 18);
            this->fcgCBAnalysisReuse->TabIndex = 52;
            this->fcgCBAnalysisReuse->Tag = L"reCmd";
            this->fcgCBAnalysisReuse->Text = L"解析結果の再利用";
            this->fcgCBAnalysisReuse->UseVisualStyleBackColor = true;
            // 
            // fcggroupBoxAnalysisReuse
            // 
            this->fcggroupBoxAnalysisReuse->Controls->Add(this->fcgNURefineInter);
            this->fcggroupBoxAnalysisReuse->Controls->Add(this->fcgNURefineIntra);
            this->fcggroupBoxAnalysisReuse->Controls->Add(this->fcgTXAnalysisRefineInter);
            this->fcggroupBoxAnalysisReuse->Controls->Add(this->fcgTXAnalysisRefineIntra);
            this->fcggroupBoxAnalysisReuse->Controls->Add(this->fcgLBAnalysisReuseLevel);
            this->fcggroupBoxAnalysisReuse->Controls->Add(this->fcgLBAnalysisReuseFile);
            this->fcggroupBoxAnalysisReuse->Controls->Add(this->fcgTXAnalysisReuseFile);
            this->fcggroupBoxAnalysisReuse->Controls->Add(this->fcgBTAnalysisReuseFile);
            this->fcggroupBoxAnalysisReuse->Controls->Add(this->fcgNUAnalysisReuseLevel);
            this->fcggroupBoxAnalysisReuse->Location = System::Drawing::Point(8, 37);
            this->fcggroupBoxAnalysisReuse->Name = L"fcggroupBoxAnalysisReuse";
            this->fcggroupBoxAnalysisReuse->Size = System::Drawing::Size(303, 88);
            this->fcggroupBoxAnalysisReuse->TabIndex = 51;
            this->fcggroupBoxAnalysisReuse->TabStop = false;
            // 
            // fcgNURefineInter
            // 
            this->fcgNURefineInter->Location = System::Drawing::Point(240, 32);
            this->fcgNURefineInter->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3, 0, 0, 0 });
            this->fcgNURefineInter->Name = L"fcgNURefineInter";
            this->fcgNURefineInter->Size = System::Drawing::Size(60, 21);
            this->fcgNURefineInter->TabIndex = 61;
            this->fcgNURefineInter->Tag = L"reCmd";
            this->fcgNURefineInter->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNURefineInter->Visible = false;
            // 
            // fcgNURefineIntra
            // 
            this->fcgNURefineIntra->Location = System::Drawing::Point(240, 9);
            this->fcgNURefineIntra->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 3, 0, 0, 0 });
            this->fcgNURefineIntra->Name = L"fcgNURefineIntra";
            this->fcgNURefineIntra->Size = System::Drawing::Size(60, 21);
            this->fcgNURefineIntra->TabIndex = 60;
            this->fcgNURefineIntra->Tag = L"reCmd";
            this->fcgNURefineIntra->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNURefineIntra->Visible = false;
            // 
            // fcgTXAnalysisRefineInter
            // 
            this->fcgTXAnalysisRefineInter->AutoSize = true;
            this->fcgTXAnalysisRefineInter->Location = System::Drawing::Point(158, 34);
            this->fcgTXAnalysisRefineInter->Name = L"fcgTXAnalysisRefineInter";
            this->fcgTXAnalysisRefineInter->Size = System::Drawing::Size(62, 14);
            this->fcgTXAnalysisRefineInter->TabIndex = 59;
            this->fcgTXAnalysisRefineInter->Text = L"インター改質";
            this->fcgTXAnalysisRefineInter->Visible = false;
            // 
            // fcgTXAnalysisRefineIntra
            // 
            this->fcgTXAnalysisRefineIntra->AutoSize = true;
            this->fcgTXAnalysisRefineIntra->Location = System::Drawing::Point(158, 10);
            this->fcgTXAnalysisRefineIntra->Name = L"fcgTXAnalysisRefineIntra";
            this->fcgTXAnalysisRefineIntra->Size = System::Drawing::Size(61, 14);
            this->fcgTXAnalysisRefineIntra->TabIndex = 58;
            this->fcgTXAnalysisRefineIntra->Text = L"イントラ改質";
            this->fcgTXAnalysisRefineIntra->Visible = false;
            // 
            // fcgLBAnalysisReuseLevel
            // 
            this->fcgLBAnalysisReuseLevel->AutoSize = true;
            this->fcgLBAnalysisReuseLevel->Location = System::Drawing::Point(6, 32);
            this->fcgLBAnalysisReuseLevel->Name = L"fcgLBAnalysisReuseLevel";
            this->fcgLBAnalysisReuseLevel->Size = System::Drawing::Size(80, 14);
            this->fcgLBAnalysisReuseLevel->TabIndex = 57;
            this->fcgLBAnalysisReuseLevel->Text = L"再利用の度合い";
            // 
            // fcgLBAnalysisReuseFile
            // 
            this->fcgLBAnalysisReuseFile->AutoSize = true;
            this->fcgLBAnalysisReuseFile->Location = System::Drawing::Point(6, 61);
            this->fcgLBAnalysisReuseFile->Name = L"fcgLBAnalysisReuseFile";
            this->fcgLBAnalysisReuseFile->Size = System::Drawing::Size(59, 14);
            this->fcgLBAnalysisReuseFile->TabIndex = 56;
            this->fcgLBAnalysisReuseFile->Text = L"解析ファイル";
            // 
            // fcgTXAnalysisReuseFile
            // 
            this->fcgTXAnalysisReuseFile->Location = System::Drawing::Point(71, 58);
            this->fcgTXAnalysisReuseFile->Name = L"fcgTXAnalysisReuseFile";
            this->fcgTXAnalysisReuseFile->Size = System::Drawing::Size(194, 21);
            this->fcgTXAnalysisReuseFile->TabIndex = 54;
            this->fcgTXAnalysisReuseFile->Tag = L"reCmd";
            this->fcgTXAnalysisReuseFile->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXAnalysisReuseFile->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            // 
            // fcgBTAnalysisReuseFile
            // 
            this->fcgBTAnalysisReuseFile->Location = System::Drawing::Point(271, 57);
            this->fcgBTAnalysisReuseFile->Name = L"fcgBTAnalysisReuseFile";
            this->fcgBTAnalysisReuseFile->Size = System::Drawing::Size(29, 22);
            this->fcgBTAnalysisReuseFile->TabIndex = 55;
            this->fcgBTAnalysisReuseFile->Text = L"...";
            this->fcgBTAnalysisReuseFile->UseVisualStyleBackColor = true;
            this->fcgBTAnalysisReuseFile->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTAnalysisReuseFile_Click);
            // 
            // fcgNUAnalysisReuseLevel
            // 
            this->fcgNUAnalysisReuseLevel->Location = System::Drawing::Point(92, 30);
            this->fcgNUAnalysisReuseLevel->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
            this->fcgNUAnalysisReuseLevel->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            this->fcgNUAnalysisReuseLevel->Name = L"fcgNUAnalysisReuseLevel";
            this->fcgNUAnalysisReuseLevel->Size = System::Drawing::Size(60, 21);
            this->fcgNUAnalysisReuseLevel->TabIndex = 53;
            this->fcgNUAnalysisReuseLevel->Tag = L"reCmd";
            this->fcgNUAnalysisReuseLevel->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            this->fcgNUAnalysisReuseLevel->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
            // 
            // fcgtabPageExSettings
            // 
            this->fcgtabPageExSettings->Controls->Add(this->fcgCBSyncProcessAffinity);
            this->fcgtabPageExSettings->Controls->Add(this->fchPNX265Sub);
            this->fcgtabPageExSettings->Controls->Add(this->fcgLBTempDir);
            this->fcgtabPageExSettings->Controls->Add(this->fcgBTCustomTempDir);
            this->fcgtabPageExSettings->Controls->Add(this->fcggroupBoxCmdEx);
            this->fcgtabPageExSettings->Controls->Add(this->fcgTXCustomTempDir);
            this->fcgtabPageExSettings->Controls->Add(this->fcgCXTempDir);
            this->fcgtabPageExSettings->Controls->Add(this->fcgCXX264Priority);
            this->fcgtabPageExSettings->Controls->Add(this->fcgLBX265Priority);
            this->fcgtabPageExSettings->Controls->Add(this->fcggroupBoxExSettings);
            this->fcgtabPageExSettings->Location = System::Drawing::Point(4, 23);
            this->fcgtabPageExSettings->Name = L"fcgtabPageExSettings";
            this->fcgtabPageExSettings->Size = System::Drawing::Size(608, 493);
            this->fcgtabPageExSettings->TabIndex = 3;
            this->fcgtabPageExSettings->Text = L"拡張";
            this->fcgtabPageExSettings->UseVisualStyleBackColor = true;
            // 
            // fcgCBSyncProcessAffinity
            // 
            this->fcgCBSyncProcessAffinity->AutoSize = true;
            this->fcgCBSyncProcessAffinity->Location = System::Drawing::Point(365, 112);
            this->fcgCBSyncProcessAffinity->Name = L"fcgCBSyncProcessAffinity";
            this->fcgCBSyncProcessAffinity->Size = System::Drawing::Size(223, 18);
            this->fcgCBSyncProcessAffinity->TabIndex = 31;
            this->fcgCBSyncProcessAffinity->Tag = L"chValue";
            this->fcgCBSyncProcessAffinity->Text = L"x265で使用するCPUコアをAviutlと同期する";
            this->fcgCBSyncProcessAffinity->UseVisualStyleBackColor = true;
            // 
            // fchPNX265Sub
            // 
            this->fchPNX265Sub->Controls->Add(this->fcgLBX265PathSub);
            this->fchPNX265Sub->Controls->Add(this->fcgTXX265PathSub);
            this->fchPNX265Sub->Controls->Add(this->fcgBTX265PathSub);
            this->fchPNX265Sub->Location = System::Drawing::Point(337, 9);
            this->fchPNX265Sub->Name = L"fchPNX265Sub";
            this->fchPNX265Sub->Size = System::Drawing::Size(264, 65);
            this->fchPNX265Sub->TabIndex = 12;
            // 
            // fcgLBX265PathSub
            // 
            this->fcgLBX265PathSub->AutoSize = true;
            this->fcgLBX265PathSub->Location = System::Drawing::Point(17, 10);
            this->fcgLBX265PathSub->Name = L"fcgLBX265PathSub";
            this->fcgLBX265PathSub->Size = System::Drawing::Size(49, 14);
            this->fcgLBX265PathSub->TabIndex = 19;
            this->fcgLBX265PathSub->Text = L"～の指定";
            // 
            // fcgTXX265PathSub
            // 
            this->fcgTXX265PathSub->AllowDrop = true;
            this->fcgTXX265PathSub->Location = System::Drawing::Point(55, 34);
            this->fcgTXX265PathSub->Name = L"fcgTXX265PathSub";
            this->fcgTXX265PathSub->Size = System::Drawing::Size(176, 21);
            this->fcgTXX265PathSub->TabIndex = 10;
            this->fcgTXX265PathSub->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTXX265PathSub_TextChanged);
            this->fcgTXX265PathSub->Enter += gcnew System::EventHandler(this, &frmConfig::fcgTXX265PathSub_Enter);
            this->fcgTXX265PathSub->Leave += gcnew System::EventHandler(this, &frmConfig::fcgTXX265PathSub_Leave);
            // 
            // fcgBTX265PathSub
            // 
            this->fcgBTX265PathSub->Location = System::Drawing::Point(235, 33);
            this->fcgBTX265PathSub->Name = L"fcgBTX265PathSub";
            this->fcgBTX265PathSub->Size = System::Drawing::Size(27, 22);
            this->fcgBTX265PathSub->TabIndex = 11;
            this->fcgBTX265PathSub->Text = L"...";
            this->fcgBTX265PathSub->UseVisualStyleBackColor = true;
            this->fcgBTX265PathSub->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTX265PathSub_Click);
            // 
            // fcgLBTempDir
            // 
            this->fcgLBTempDir->AutoSize = true;
            this->fcgLBTempDir->Location = System::Drawing::Point(362, 154);
            this->fcgLBTempDir->Name = L"fcgLBTempDir";
            this->fcgLBTempDir->Size = System::Drawing::Size(60, 14);
            this->fcgLBTempDir->TabIndex = 4;
            this->fcgLBTempDir->Text = L"一時フォルダ";
            // 
            // fcgBTCustomTempDir
            // 
            this->fcgBTCustomTempDir->Location = System::Drawing::Point(562, 207);
            this->fcgBTCustomTempDir->Name = L"fcgBTCustomTempDir";
            this->fcgBTCustomTempDir->Size = System::Drawing::Size(29, 23);
            this->fcgBTCustomTempDir->TabIndex = 22;
            this->fcgBTCustomTempDir->Text = L"...";
            this->fcgBTCustomTempDir->UseVisualStyleBackColor = true;
            this->fcgBTCustomTempDir->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTCustomTempDir_Click);
            // 
            // fcggroupBoxCmdEx
            // 
            this->fcggroupBoxCmdEx->Controls->Add(this->fcgCXCmdExInsert);
            this->fcggroupBoxCmdEx->Controls->Add(this->fcgCBNulOutCLI);
            this->fcggroupBoxCmdEx->Controls->Add(this->fcgBTCmdEx);
            this->fcggroupBoxCmdEx->Controls->Add(this->fcgTXCmdEx);
            this->fcggroupBoxCmdEx->Location = System::Drawing::Point(8, 243);
            this->fcggroupBoxCmdEx->Name = L"fcggroupBoxCmdEx";
            this->fcggroupBoxCmdEx->Size = System::Drawing::Size(589, 233);
            this->fcggroupBoxCmdEx->TabIndex = 30;
            this->fcggroupBoxCmdEx->TabStop = false;
            this->fcggroupBoxCmdEx->Text = L"追加コマンド";
            // 
            // fcgCXCmdExInsert
            // 
            this->fcgCXCmdExInsert->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXCmdExInsert->FormattingEnabled = true;
            this->fcgCXCmdExInsert->Location = System::Drawing::Point(197, 204);
            this->fcgCXCmdExInsert->Name = L"fcgCXCmdExInsert";
            this->fcgCXCmdExInsert->Size = System::Drawing::Size(168, 22);
            this->fcgCXCmdExInsert->TabIndex = 33;
            this->fcgCXCmdExInsert->SelectedIndexChanged += gcnew System::EventHandler(this, &frmConfig::fcgCXCmdExInsert_SelectedIndexChanged);
            this->fcgCXCmdExInsert->FontChanged += gcnew System::EventHandler(this, &frmConfig::fcgCXCmdExInsert_FontChanged);
            // 
            // fcgCBNulOutCLI
            // 
            this->fcgCBNulOutCLI->AutoSize = true;
            this->fcgCBNulOutCLI->Location = System::Drawing::Point(18, 206);
            this->fcgCBNulOutCLI->Name = L"fcgCBNulOutCLI";
            this->fcgCBNulOutCLI->Size = System::Drawing::Size(65, 18);
            this->fcgCBNulOutCLI->TabIndex = 32;
            this->fcgCBNulOutCLI->Tag = L"chValue";
            this->fcgCBNulOutCLI->Text = L"nul出力";
            this->fcgCBNulOutCLI->UseVisualStyleBackColor = true;
            // 
            // fcgBTCmdEx
            // 
            this->fcgBTCmdEx->Location = System::Drawing::Point(482, 204);
            this->fcgBTCmdEx->Name = L"fcgBTCmdEx";
            this->fcgBTCmdEx->Size = System::Drawing::Size(97, 23);
            this->fcgBTCmdEx->TabIndex = 34;
            this->fcgBTCmdEx->Text = L"GUI部に反映";
            this->fcgBTCmdEx->UseVisualStyleBackColor = true;
            this->fcgBTCmdEx->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTCmdEx_Click);
            // 
            // fcgTXCmdEx
            // 
            this->fcgTXCmdEx->AllowDrop = true;
            this->fcgTXCmdEx->Font = (gcnew System::Drawing::Font(L"ＭＳ ゴシック", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->fcgTXCmdEx->Location = System::Drawing::Point(6, 20);
            this->fcgTXCmdEx->Multiline = true;
            this->fcgTXCmdEx->Name = L"fcgTXCmdEx";
            this->fcgTXCmdEx->Size = System::Drawing::Size(577, 180);
            this->fcgTXCmdEx->TabIndex = 31;
            this->fcgTXCmdEx->Tag = L"chValue";
            this->fcgTXCmdEx->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgInsertDragDropFilename_DragDrop);
            this->fcgTXCmdEx->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgInsertDragDropFilename_Enter);
            this->fcgTXCmdEx->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgInsertDragDropFilename_DragOver);
            // 
            // fcgTXCustomTempDir
            // 
            this->fcgTXCustomTempDir->Location = System::Drawing::Point(377, 208);
            this->fcgTXCustomTempDir->Name = L"fcgTXCustomTempDir";
            this->fcgTXCustomTempDir->Size = System::Drawing::Size(182, 21);
            this->fcgTXCustomTempDir->TabIndex = 21;
            this->fcgTXCustomTempDir->Tag = L"";
            this->fcgTXCustomTempDir->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTXCustomTempDir_TextChanged);
            // 
            // fcgCXTempDir
            // 
            this->fcgCXTempDir->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXTempDir->FormattingEnabled = true;
            this->fcgCXTempDir->Location = System::Drawing::Point(365, 180);
            this->fcgCXTempDir->Name = L"fcgCXTempDir";
            this->fcgCXTempDir->Size = System::Drawing::Size(209, 22);
            this->fcgCXTempDir->TabIndex = 20;
            this->fcgCXTempDir->Tag = L"chValue";
            // 
            // fcgCXX264Priority
            // 
            this->fcgCXX264Priority->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXX264Priority->FormattingEnabled = true;
            this->fcgCXX264Priority->Location = System::Drawing::Point(435, 84);
            this->fcgCXX264Priority->Name = L"fcgCXX264Priority";
            this->fcgCXX264Priority->Size = System::Drawing::Size(152, 22);
            this->fcgCXX264Priority->TabIndex = 15;
            this->fcgCXX264Priority->Tag = L"chValue";
            // 
            // fcgLBX265Priority
            // 
            this->fcgLBX265Priority->AutoSize = true;
            this->fcgLBX265Priority->Location = System::Drawing::Point(347, 87);
            this->fcgLBX265Priority->Name = L"fcgLBX265Priority";
            this->fcgLBX265Priority->Size = System::Drawing::Size(67, 14);
            this->fcgLBX265Priority->TabIndex = 1;
            this->fcgLBX265Priority->Text = L"x265優先度";
            // 
            // fcggroupBoxExSettings
            // 
            this->fcggroupBoxExSettings->Controls->Add(this->fcgCBSetKeyframeAtChapter);
            this->fcggroupBoxExSettings->Controls->Add(this->fcgCBInputAsLW48);
            this->fcggroupBoxExSettings->Controls->Add(this->fcgCBCheckKeyframes);
            this->fcggroupBoxExSettings->Controls->Add(this->fcgCBAuoTcfileout);
            this->fcggroupBoxExSettings->Controls->Add(this->fcgCBAFSBitrateCorrection);
            this->fcggroupBoxExSettings->Controls->Add(this->fcgCBAFS);
            this->fcggroupBoxExSettings->Location = System::Drawing::Point(8, 3);
            this->fcggroupBoxExSettings->Name = L"fcggroupBoxExSettings";
            this->fcggroupBoxExSettings->Size = System::Drawing::Size(323, 234);
            this->fcggroupBoxExSettings->TabIndex = 0;
            this->fcggroupBoxExSettings->TabStop = false;
            this->fcggroupBoxExSettings->Text = L"拡張設定";
            // 
            // fcgCBSetKeyframeAtChapter
            // 
            this->fcgCBSetKeyframeAtChapter->AutoSize = true;
            this->fcgCBSetKeyframeAtChapter->Location = System::Drawing::Point(18, 140);
            this->fcgCBSetKeyframeAtChapter->Name = L"fcgCBSetKeyframeAtChapter";
            this->fcgCBSetKeyframeAtChapter->Size = System::Drawing::Size(267, 18);
            this->fcgCBSetKeyframeAtChapter->TabIndex = 5;
            this->fcgCBSetKeyframeAtChapter->Tag = L"chValue";
            this->fcgCBSetKeyframeAtChapter->Text = L"チャプター位置にキーフレームを設定する (mux有効時)";
            this->fcgCBSetKeyframeAtChapter->UseVisualStyleBackColor = true;
            // 
            // fcgCBInputAsLW48
            // 
            this->fcgCBInputAsLW48->AutoSize = true;
            this->fcgCBInputAsLW48->Location = System::Drawing::Point(18, 171);
            this->fcgCBInputAsLW48->Name = L"fcgCBInputAsLW48";
            this->fcgCBInputAsLW48->Size = System::Drawing::Size(81, 18);
            this->fcgCBInputAsLW48->TabIndex = 6;
            this->fcgCBInputAsLW48->Tag = L"chValue";
            this->fcgCBInputAsLW48->Text = L"LW48モード";
            this->fcgCBInputAsLW48->UseVisualStyleBackColor = true;
            // 
            // fcgCBCheckKeyframes
            // 
            this->fcgCBCheckKeyframes->AutoSize = true;
            this->fcgCBCheckKeyframes->Location = System::Drawing::Point(18, 116);
            this->fcgCBCheckKeyframes->Name = L"fcgCBCheckKeyframes";
            this->fcgCBCheckKeyframes->Size = System::Drawing::Size(186, 18);
            this->fcgCBCheckKeyframes->TabIndex = 4;
            this->fcgCBCheckKeyframes->Tag = L"chValue";
            this->fcgCBCheckKeyframes->Text = L"Aviutlのキーフレーム設定検出を行う";
            this->fcgCBCheckKeyframes->UseVisualStyleBackColor = true;
            // 
            // fcgCBAuoTcfileout
            // 
            this->fcgCBAuoTcfileout->AutoSize = true;
            this->fcgCBAuoTcfileout->Location = System::Drawing::Point(18, 92);
            this->fcgCBAuoTcfileout->Name = L"fcgCBAuoTcfileout";
            this->fcgCBAuoTcfileout->Size = System::Drawing::Size(98, 18);
            this->fcgCBAuoTcfileout->TabIndex = 3;
            this->fcgCBAuoTcfileout->Tag = L"chValue";
            this->fcgCBAuoTcfileout->Text = L"タイムコード出力";
            this->fcgCBAuoTcfileout->UseVisualStyleBackColor = true;
            // 
            // fcgCBAFSBitrateCorrection
            // 
            this->fcgCBAFSBitrateCorrection->AutoSize = true;
            this->fcgCBAFSBitrateCorrection->Location = System::Drawing::Point(34, 53);
            this->fcgCBAFSBitrateCorrection->Name = L"fcgCBAFSBitrateCorrection";
            this->fcgCBAFSBitrateCorrection->Size = System::Drawing::Size(278, 18);
            this->fcgCBAFSBitrateCorrection->TabIndex = 2;
            this->fcgCBAFSBitrateCorrection->Tag = L"chValue";
            this->fcgCBAFSBitrateCorrection->Text = L"設定したビットレートになるよう補正する(自動マルチパス時)";
            this->fcgCBAFSBitrateCorrection->UseVisualStyleBackColor = true;
            // 
            // fcgCBAFS
            // 
            this->fcgCBAFS->AutoSize = true;
            this->fcgCBAFS->Location = System::Drawing::Point(18, 29);
            this->fcgCBAFS->Name = L"fcgCBAFS";
            this->fcgCBAFS->Size = System::Drawing::Size(183, 18);
            this->fcgCBAFS->TabIndex = 1;
            this->fcgCBAFS->Tag = L"chValue";
            this->fcgCBAFS->Text = L"自動フィールドシフト(afs)を使用する";
            this->fcgCBAFS->UseVisualStyleBackColor = true;
            this->fcgCBAFS->CheckedChanged += gcnew System::EventHandler(this, &frmConfig::fcgCBAFS_CheckedChanged);
            // 
            // fcgtoolStripSettings
            // 
            this->fcgtoolStripSettings->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->fcgtoolStripSettings->ImageScalingSize = System::Drawing::Size(18, 18);
            this->fcgtoolStripSettings->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(15) {
                this->fcgTSBSave,
                    this->fcgTSBSaveNew, this->fcgTSBDelete, this->fcgtoolStripSeparator1, this->fcgTSSettings, this->fcgTSBCMDOnly, this->toolStripSeparator1,
                    this->fcgTSLanguage, this->toolStripSeparator3, this->fcgTSBBitrateCalc, this->toolStripSeparator6, this->fcgTSBOtherSettings,
                    this->fcgTSLSettingsNotes, this->fcgTSTSettingsNotes, this->toolStripSeparator4
            });
            this->fcgtoolStripSettings->Location = System::Drawing::Point(0, 0);
            this->fcgtoolStripSettings->Name = L"fcgtoolStripSettings";
            this->fcgtoolStripSettings->Padding = System::Windows::Forms::Padding(0);
            this->fcgtoolStripSettings->Size = System::Drawing::Size(1008, 25);
            this->fcgtoolStripSettings->TabIndex = 1;
            this->fcgtoolStripSettings->Text = L"toolStrip1";
            // 
            // fcgTSBSave
            // 
            this->fcgTSBSave->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"fcgTSBSave.Image")));
            this->fcgTSBSave->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->fcgTSBSave->Name = L"fcgTSBSave";
            this->fcgTSBSave->Size = System::Drawing::Size(86, 22);
            this->fcgTSBSave->Text = L"上書き保存";
            this->fcgTSBSave->Click += gcnew System::EventHandler(this, &frmConfig::fcgTSBSave_Click);
            // 
            // fcgTSBSaveNew
            // 
            this->fcgTSBSaveNew->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"fcgTSBSaveNew.Image")));
            this->fcgTSBSaveNew->ImageTransparentColor = System::Drawing::Color::Black;
            this->fcgTSBSaveNew->Name = L"fcgTSBSaveNew";
            this->fcgTSBSaveNew->Size = System::Drawing::Size(77, 22);
            this->fcgTSBSaveNew->Text = L"新規保存";
            this->fcgTSBSaveNew->Click += gcnew System::EventHandler(this, &frmConfig::fcgTSBSaveNew_Click);
            // 
            // fcgTSBDelete
            // 
            this->fcgTSBDelete->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"fcgTSBDelete.Image")));
            this->fcgTSBDelete->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->fcgTSBDelete->Name = L"fcgTSBDelete";
            this->fcgTSBDelete->Size = System::Drawing::Size(53, 22);
            this->fcgTSBDelete->Text = L"削除";
            this->fcgTSBDelete->Click += gcnew System::EventHandler(this, &frmConfig::fcgTSBDelete_Click);
            // 
            // fcgtoolStripSeparator1
            // 
            this->fcgtoolStripSeparator1->Name = L"fcgtoolStripSeparator1";
            this->fcgtoolStripSeparator1->Size = System::Drawing::Size(6, 25);
            // 
            // fcgTSSettings
            // 
            this->fcgTSSettings->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"fcgTSSettings.Image")));
            this->fcgTSSettings->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->fcgTSSettings->Name = L"fcgTSSettings";
            this->fcgTSSettings->Size = System::Drawing::Size(79, 22);
            this->fcgTSSettings->Text = L"プリセット";
            this->fcgTSSettings->DropDownItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &frmConfig::fcgTSSettings_DropDownItemClicked);
            this->fcgTSSettings->Click += gcnew System::EventHandler(this, &frmConfig::fcgTSSettings_Click);
            // 
            // fcgTSBCMDOnly
            // 
            this->fcgTSBCMDOnly->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
            this->fcgTSBCMDOnly->CheckOnClick = true;
            this->fcgTSBCMDOnly->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"fcgTSBCMDOnly.Image")));
            this->fcgTSBCMDOnly->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->fcgTSBCMDOnly->Name = L"fcgTSBCMDOnly";
            this->fcgTSBCMDOnly->Size = System::Drawing::Size(76, 22);
            this->fcgTSBCMDOnly->Tag = L"chValue";
            this->fcgTSBCMDOnly->Text = L"CLIモード";
            // 
            // toolStripSeparator1
            // 
            this->toolStripSeparator1->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
            this->toolStripSeparator1->Name = L"toolStripSeparator1";
            this->toolStripSeparator1->Size = System::Drawing::Size(6, 25);
            // 
            // fcgTSLanguage
            // 
            this->fcgTSLanguage->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
            this->fcgTSLanguage->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->fcgTSLanguage->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"fcgTSLanguage.Image")));
            this->fcgTSLanguage->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->fcgTSLanguage->Name = L"fcgTSLanguage";
            this->fcgTSLanguage->Size = System::Drawing::Size(44, 22);
            this->fcgTSLanguage->Text = L"言語";
            this->fcgTSLanguage->ToolTipText = L"fcgTSLanguage";
            this->fcgTSLanguage->DropDownItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &frmConfig::fcgTSLanguage_DropDownItemClicked);
            // 
            // toolStripSeparator3
            // 
            this->toolStripSeparator3->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
            this->toolStripSeparator3->Name = L"toolStripSeparator3";
            this->toolStripSeparator3->Size = System::Drawing::Size(6, 25);
            // 
            // fcgTSBBitrateCalc
            // 
            this->fcgTSBBitrateCalc->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
            this->fcgTSBBitrateCalc->CheckOnClick = true;
            this->fcgTSBBitrateCalc->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->fcgTSBBitrateCalc->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"fcgTSBBitrateCalc.Image")));
            this->fcgTSBBitrateCalc->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->fcgTSBBitrateCalc->Name = L"fcgTSBBitrateCalc";
            this->fcgTSBBitrateCalc->Size = System::Drawing::Size(97, 22);
            this->fcgTSBBitrateCalc->Text = L"ビットレート計算機";
            this->fcgTSBBitrateCalc->CheckedChanged += gcnew System::EventHandler(this, &frmConfig::fcgTSBBitrateCalc_CheckedChanged);
            // 
            // toolStripSeparator6
            // 
            this->toolStripSeparator6->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
            this->toolStripSeparator6->Name = L"toolStripSeparator6";
            this->toolStripSeparator6->Size = System::Drawing::Size(6, 25);
            // 
            // fcgTSBOtherSettings
            // 
            this->fcgTSBOtherSettings->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
            this->fcgTSBOtherSettings->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
            this->fcgTSBOtherSettings->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"fcgTSBOtherSettings.Image")));
            this->fcgTSBOtherSettings->ImageTransparentColor = System::Drawing::Color::Magenta;
            this->fcgTSBOtherSettings->Name = L"fcgTSBOtherSettings";
            this->fcgTSBOtherSettings->Size = System::Drawing::Size(76, 22);
            this->fcgTSBOtherSettings->Text = L"その他の設定";
            this->fcgTSBOtherSettings->Click += gcnew System::EventHandler(this, &frmConfig::fcgTSBOtherSettings_Click);
            // 
            // fcgTSLSettingsNotes
            // 
            this->fcgTSLSettingsNotes->DoubleClickEnabled = true;
            this->fcgTSLSettingsNotes->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->fcgTSLSettingsNotes->Margin = System::Windows::Forms::Padding(3, 1, 0, 2);
            this->fcgTSLSettingsNotes->Name = L"fcgTSLSettingsNotes";
            this->fcgTSLSettingsNotes->Overflow = System::Windows::Forms::ToolStripItemOverflow::Never;
            this->fcgTSLSettingsNotes->Size = System::Drawing::Size(45, 22);
            this->fcgTSLSettingsNotes->Text = L"メモ表示";
            this->fcgTSLSettingsNotes->DoubleClick += gcnew System::EventHandler(this, &frmConfig::fcgTSLSettingsNotes_DoubleClick);
            // 
            // fcgTSTSettingsNotes
            // 
            this->fcgTSTSettingsNotes->BackColor = System::Drawing::SystemColors::Window;
            this->fcgTSTSettingsNotes->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->fcgTSTSettingsNotes->Margin = System::Windows::Forms::Padding(3, 0, 1, 0);
            this->fcgTSTSettingsNotes->Name = L"fcgTSTSettingsNotes";
            this->fcgTSTSettingsNotes->Size = System::Drawing::Size(200, 25);
            this->fcgTSTSettingsNotes->Text = L"メモ...";
            this->fcgTSTSettingsNotes->Visible = false;
            this->fcgTSTSettingsNotes->Leave += gcnew System::EventHandler(this, &frmConfig::fcgTSTSettingsNotes_Leave);
            this->fcgTSTSettingsNotes->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &frmConfig::fcgTSTSettingsNotes_KeyDown);
            this->fcgTSTSettingsNotes->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTSTSettingsNotes_TextChanged);
            // 
            // toolStripSeparator4
            // 
            this->toolStripSeparator4->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
            this->toolStripSeparator4->Name = L"toolStripSeparator4";
            this->toolStripSeparator4->Size = System::Drawing::Size(6, 25);
            // 
            // fcgtabControlMux
            // 
            this->fcgtabControlMux->Controls->Add(this->fcgtabPageMP4);
            this->fcgtabControlMux->Controls->Add(this->fcgtabPageMKV);
            this->fcgtabControlMux->Controls->Add(this->fcgtabPageMux);
            this->fcgtabControlMux->Controls->Add(this->fcgtabPageBat);
            this->fcgtabControlMux->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->fcgtabControlMux->Location = System::Drawing::Point(2, 2);
            this->fcgtabControlMux->Name = L"fcgtabControlMux";
            this->fcgtabControlMux->SelectedIndex = 0;
            this->fcgtabControlMux->Size = System::Drawing::Size(384, 214);
            this->fcgtabControlMux->TabIndex = 3;
            // 
            // fcgtabPageMP4
            // 
            this->fcgtabPageMP4->Controls->Add(this->fcgCBMP4MuxApple);
            this->fcgtabPageMP4->Controls->Add(this->fcgBTTC2MP4Path);
            this->fcgtabPageMP4->Controls->Add(this->fcgTXTC2MP4Path);
            this->fcgtabPageMP4->Controls->Add(this->fcgBTMP4MuxerPath);
            this->fcgtabPageMP4->Controls->Add(this->fcgTXMP4MuxerPath);
            this->fcgtabPageMP4->Controls->Add(this->fcgLBTC2MP4Path);
            this->fcgtabPageMP4->Controls->Add(this->fcgLBMP4MuxerPath);
            this->fcgtabPageMP4->Controls->Add(this->fcgCXMP4CmdEx);
            this->fcgtabPageMP4->Controls->Add(this->fcgLBMP4CmdEx);
            this->fcgtabPageMP4->Controls->Add(this->fcgCBMP4MuxerExt);
            this->fcgtabPageMP4->Controls->Add(this->fcgBTMP4RawPath);
            this->fcgtabPageMP4->Controls->Add(this->fcgTXMP4RawPath);
            this->fcgtabPageMP4->Controls->Add(this->fcgLBMP4RawPath);
            this->fcgtabPageMP4->Controls->Add(this->fcgBTMP4BoxTempDir);
            this->fcgtabPageMP4->Controls->Add(this->fcgTXMP4BoxTempDir);
            this->fcgtabPageMP4->Controls->Add(this->fcgCXMP4BoxTempDir);
            this->fcgtabPageMP4->Controls->Add(this->fcgLBMP4BoxTempDir);
            this->fcgtabPageMP4->Location = System::Drawing::Point(4, 23);
            this->fcgtabPageMP4->Name = L"fcgtabPageMP4";
            this->fcgtabPageMP4->Padding = System::Windows::Forms::Padding(3);
            this->fcgtabPageMP4->Size = System::Drawing::Size(376, 187);
            this->fcgtabPageMP4->TabIndex = 0;
            this->fcgtabPageMP4->Text = L"mp4";
            this->fcgtabPageMP4->UseVisualStyleBackColor = true;
            // 
            // fcgCBMP4MuxApple
            // 
            this->fcgCBMP4MuxApple->AutoSize = true;
            this->fcgCBMP4MuxApple->Location = System::Drawing::Point(261, 37);
            this->fcgCBMP4MuxApple->Name = L"fcgCBMP4MuxApple";
            this->fcgCBMP4MuxApple->Size = System::Drawing::Size(109, 18);
            this->fcgCBMP4MuxApple->TabIndex = 2;
            this->fcgCBMP4MuxApple->Tag = L"chValue";
            this->fcgCBMP4MuxApple->Text = L"Apple形式に対応";
            this->fcgCBMP4MuxApple->UseVisualStyleBackColor = true;
            // 
            // fcgBTTC2MP4Path
            // 
            this->fcgBTTC2MP4Path->Location = System::Drawing::Point(340, 87);
            this->fcgBTTC2MP4Path->Name = L"fcgBTTC2MP4Path";
            this->fcgBTTC2MP4Path->Size = System::Drawing::Size(30, 23);
            this->fcgBTTC2MP4Path->TabIndex = 6;
            this->fcgBTTC2MP4Path->Text = L"...";
            this->fcgBTTC2MP4Path->UseVisualStyleBackColor = true;
            this->fcgBTTC2MP4Path->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTTC2MP4Path_Click);
            // 
            // fcgTXTC2MP4Path
            // 
            this->fcgTXTC2MP4Path->AllowDrop = true;
            this->fcgTXTC2MP4Path->Location = System::Drawing::Point(136, 88);
            this->fcgTXTC2MP4Path->Name = L"fcgTXTC2MP4Path";
            this->fcgTXTC2MP4Path->Size = System::Drawing::Size(202, 21);
            this->fcgTXTC2MP4Path->TabIndex = 5;
            this->fcgTXTC2MP4Path->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTXTC2MP4Path_TextChanged);
            this->fcgTXTC2MP4Path->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXTC2MP4Path->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            this->fcgTXTC2MP4Path->Enter += gcnew System::EventHandler(this, &frmConfig::fcgTXTC2MP4Path_Enter);
            this->fcgTXTC2MP4Path->Leave += gcnew System::EventHandler(this, &frmConfig::fcgTXTC2MP4Path_Leave);
            // 
            // fcgBTMP4MuxerPath
            // 
            this->fcgBTMP4MuxerPath->Location = System::Drawing::Point(340, 65);
            this->fcgBTMP4MuxerPath->Name = L"fcgBTMP4MuxerPath";
            this->fcgBTMP4MuxerPath->Size = System::Drawing::Size(30, 23);
            this->fcgBTMP4MuxerPath->TabIndex = 4;
            this->fcgBTMP4MuxerPath->Text = L"...";
            this->fcgBTMP4MuxerPath->UseVisualStyleBackColor = true;
            this->fcgBTMP4MuxerPath->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTMP4MuxerPath_Click);
            // 
            // fcgTXMP4MuxerPath
            // 
            this->fcgTXMP4MuxerPath->AllowDrop = true;
            this->fcgTXMP4MuxerPath->Location = System::Drawing::Point(136, 66);
            this->fcgTXMP4MuxerPath->Name = L"fcgTXMP4MuxerPath";
            this->fcgTXMP4MuxerPath->Size = System::Drawing::Size(202, 21);
            this->fcgTXMP4MuxerPath->TabIndex = 3;
            this->fcgTXMP4MuxerPath->Tag = L"";
            this->fcgTXMP4MuxerPath->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTXMP4MuxerPath_TextChanged);
            this->fcgTXMP4MuxerPath->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXMP4MuxerPath->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            this->fcgTXMP4MuxerPath->Enter += gcnew System::EventHandler(this, &frmConfig::fcgTXMP4MuxerPath_Enter);
            this->fcgTXMP4MuxerPath->Leave += gcnew System::EventHandler(this, &frmConfig::fcgTXMP4MuxerPath_Leave);
            // 
            // fcgLBTC2MP4Path
            // 
            this->fcgLBTC2MP4Path->AutoSize = true;
            this->fcgLBTC2MP4Path->Location = System::Drawing::Point(4, 91);
            this->fcgLBTC2MP4Path->Name = L"fcgLBTC2MP4Path";
            this->fcgLBTC2MP4Path->Size = System::Drawing::Size(49, 14);
            this->fcgLBTC2MP4Path->TabIndex = 4;
            this->fcgLBTC2MP4Path->Text = L"～の指定";
            // 
            // fcgLBMP4MuxerPath
            // 
            this->fcgLBMP4MuxerPath->AutoSize = true;
            this->fcgLBMP4MuxerPath->Location = System::Drawing::Point(4, 69);
            this->fcgLBMP4MuxerPath->Name = L"fcgLBMP4MuxerPath";
            this->fcgLBMP4MuxerPath->Size = System::Drawing::Size(49, 14);
            this->fcgLBMP4MuxerPath->TabIndex = 3;
            this->fcgLBMP4MuxerPath->Text = L"～の指定";
            // 
            // fcgCXMP4CmdEx
            // 
            this->fcgCXMP4CmdEx->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXMP4CmdEx->FormattingEnabled = true;
            this->fcgCXMP4CmdEx->Location = System::Drawing::Point(213, 9);
            this->fcgCXMP4CmdEx->Name = L"fcgCXMP4CmdEx";
            this->fcgCXMP4CmdEx->Size = System::Drawing::Size(157, 22);
            this->fcgCXMP4CmdEx->TabIndex = 1;
            this->fcgCXMP4CmdEx->Tag = L"chValue";
            // 
            // fcgLBMP4CmdEx
            // 
            this->fcgLBMP4CmdEx->AutoSize = true;
            this->fcgLBMP4CmdEx->Location = System::Drawing::Point(139, 12);
            this->fcgLBMP4CmdEx->Name = L"fcgLBMP4CmdEx";
            this->fcgLBMP4CmdEx->Size = System::Drawing::Size(68, 14);
            this->fcgLBMP4CmdEx->TabIndex = 1;
            this->fcgLBMP4CmdEx->Text = L"拡張オプション";
            // 
            // fcgCBMP4MuxerExt
            // 
            this->fcgCBMP4MuxerExt->AutoSize = true;
            this->fcgCBMP4MuxerExt->Location = System::Drawing::Point(10, 11);
            this->fcgCBMP4MuxerExt->Name = L"fcgCBMP4MuxerExt";
            this->fcgCBMP4MuxerExt->Size = System::Drawing::Size(113, 18);
            this->fcgCBMP4MuxerExt->TabIndex = 0;
            this->fcgCBMP4MuxerExt->Tag = L"chValue";
            this->fcgCBMP4MuxerExt->Text = L"外部muxerを使用";
            this->fcgCBMP4MuxerExt->UseVisualStyleBackColor = true;
            // 
            // fcgBTMP4RawPath
            // 
            this->fcgBTMP4RawPath->Location = System::Drawing::Point(340, 109);
            this->fcgBTMP4RawPath->Name = L"fcgBTMP4RawPath";
            this->fcgBTMP4RawPath->Size = System::Drawing::Size(30, 23);
            this->fcgBTMP4RawPath->TabIndex = 8;
            this->fcgBTMP4RawPath->Text = L"...";
            this->fcgBTMP4RawPath->UseVisualStyleBackColor = true;
            this->fcgBTMP4RawPath->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTMP4RawMuxerPath_Click);
            // 
            // fcgTXMP4RawPath
            // 
            this->fcgTXMP4RawPath->AllowDrop = true;
            this->fcgTXMP4RawPath->Location = System::Drawing::Point(136, 110);
            this->fcgTXMP4RawPath->Name = L"fcgTXMP4RawPath";
            this->fcgTXMP4RawPath->Size = System::Drawing::Size(202, 21);
            this->fcgTXMP4RawPath->TabIndex = 7;
            this->fcgTXMP4RawPath->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTXMP4RawMuxerPath_TextChanged);
            this->fcgTXMP4RawPath->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXMP4RawPath->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            this->fcgTXMP4RawPath->Enter += gcnew System::EventHandler(this, &frmConfig::fcgTXMP4RawPath_Enter);
            this->fcgTXMP4RawPath->Leave += gcnew System::EventHandler(this, &frmConfig::fcgTXMP4RawPath_Leave);
            // 
            // fcgLBMP4RawPath
            // 
            this->fcgLBMP4RawPath->AutoSize = true;
            this->fcgLBMP4RawPath->Location = System::Drawing::Point(4, 113);
            this->fcgLBMP4RawPath->Name = L"fcgLBMP4RawPath";
            this->fcgLBMP4RawPath->Size = System::Drawing::Size(49, 14);
            this->fcgLBMP4RawPath->TabIndex = 20;
            this->fcgLBMP4RawPath->Text = L"～の指定";
            // 
            // fcgBTMP4BoxTempDir
            // 
            this->fcgBTMP4BoxTempDir->Location = System::Drawing::Point(340, 159);
            this->fcgBTMP4BoxTempDir->Name = L"fcgBTMP4BoxTempDir";
            this->fcgBTMP4BoxTempDir->Size = System::Drawing::Size(30, 23);
            this->fcgBTMP4BoxTempDir->TabIndex = 11;
            this->fcgBTMP4BoxTempDir->Text = L"...";
            this->fcgBTMP4BoxTempDir->UseVisualStyleBackColor = true;
            this->fcgBTMP4BoxTempDir->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTMP4BoxTempDir_Click);
            // 
            // fcgTXMP4BoxTempDir
            // 
            this->fcgTXMP4BoxTempDir->Location = System::Drawing::Point(107, 160);
            this->fcgTXMP4BoxTempDir->Name = L"fcgTXMP4BoxTempDir";
            this->fcgTXMP4BoxTempDir->Size = System::Drawing::Size(227, 21);
            this->fcgTXMP4BoxTempDir->TabIndex = 10;
            this->fcgTXMP4BoxTempDir->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTXMP4BoxTempDir_TextChanged);
            // 
            // fcgCXMP4BoxTempDir
            // 
            this->fcgCXMP4BoxTempDir->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXMP4BoxTempDir->FormattingEnabled = true;
            this->fcgCXMP4BoxTempDir->Location = System::Drawing::Point(145, 132);
            this->fcgCXMP4BoxTempDir->Name = L"fcgCXMP4BoxTempDir";
            this->fcgCXMP4BoxTempDir->Size = System::Drawing::Size(206, 22);
            this->fcgCXMP4BoxTempDir->TabIndex = 9;
            this->fcgCXMP4BoxTempDir->Tag = L"chValue";
            // 
            // fcgLBMP4BoxTempDir
            // 
            this->fcgLBMP4BoxTempDir->AutoSize = true;
            this->fcgLBMP4BoxTempDir->Location = System::Drawing::Point(25, 135);
            this->fcgLBMP4BoxTempDir->Name = L"fcgLBMP4BoxTempDir";
            this->fcgLBMP4BoxTempDir->Size = System::Drawing::Size(105, 14);
            this->fcgLBMP4BoxTempDir->TabIndex = 18;
            this->fcgLBMP4BoxTempDir->Text = L"mp4box一時フォルダ";
            // 
            // fcgtabPageMKV
            // 
            this->fcgtabPageMKV->Controls->Add(this->fcgBTMKVMuxerPath);
            this->fcgtabPageMKV->Controls->Add(this->fcgTXMKVMuxerPath);
            this->fcgtabPageMKV->Controls->Add(this->fcgLBMKVMuxerPath);
            this->fcgtabPageMKV->Controls->Add(this->fcgCXMKVCmdEx);
            this->fcgtabPageMKV->Controls->Add(this->fcgLBMKVMuxerCmdEx);
            this->fcgtabPageMKV->Controls->Add(this->fcgCBMKVMuxerExt);
            this->fcgtabPageMKV->Location = System::Drawing::Point(4, 23);
            this->fcgtabPageMKV->Name = L"fcgtabPageMKV";
            this->fcgtabPageMKV->Padding = System::Windows::Forms::Padding(3);
            this->fcgtabPageMKV->Size = System::Drawing::Size(376, 187);
            this->fcgtabPageMKV->TabIndex = 1;
            this->fcgtabPageMKV->Text = L"mkv";
            this->fcgtabPageMKV->UseVisualStyleBackColor = true;
            // 
            // fcgBTMKVMuxerPath
            // 
            this->fcgBTMKVMuxerPath->Location = System::Drawing::Point(340, 76);
            this->fcgBTMKVMuxerPath->Name = L"fcgBTMKVMuxerPath";
            this->fcgBTMKVMuxerPath->Size = System::Drawing::Size(30, 23);
            this->fcgBTMKVMuxerPath->TabIndex = 3;
            this->fcgBTMKVMuxerPath->Text = L"...";
            this->fcgBTMKVMuxerPath->UseVisualStyleBackColor = true;
            this->fcgBTMKVMuxerPath->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTMKVMuxerPath_Click);
            // 
            // fcgTXMKVMuxerPath
            // 
            this->fcgTXMKVMuxerPath->Location = System::Drawing::Point(131, 77);
            this->fcgTXMKVMuxerPath->Name = L"fcgTXMKVMuxerPath";
            this->fcgTXMKVMuxerPath->Size = System::Drawing::Size(207, 21);
            this->fcgTXMKVMuxerPath->TabIndex = 2;
            this->fcgTXMKVMuxerPath->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTXMKVMuxerPath_TextChanged);
            this->fcgTXMKVMuxerPath->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXMKVMuxerPath->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            this->fcgTXMKVMuxerPath->Enter += gcnew System::EventHandler(this, &frmConfig::fcgTXMKVMuxerPath_Enter);
            this->fcgTXMKVMuxerPath->Leave += gcnew System::EventHandler(this, &frmConfig::fcgTXMKVMuxerPath_Leave);
            // 
            // fcgLBMKVMuxerPath
            // 
            this->fcgLBMKVMuxerPath->AutoSize = true;
            this->fcgLBMKVMuxerPath->Location = System::Drawing::Point(4, 80);
            this->fcgLBMKVMuxerPath->Name = L"fcgLBMKVMuxerPath";
            this->fcgLBMKVMuxerPath->Size = System::Drawing::Size(49, 14);
            this->fcgLBMKVMuxerPath->TabIndex = 19;
            this->fcgLBMKVMuxerPath->Text = L"～の指定";
            // 
            // fcgCXMKVCmdEx
            // 
            this->fcgCXMKVCmdEx->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXMKVCmdEx->FormattingEnabled = true;
            this->fcgCXMKVCmdEx->Location = System::Drawing::Point(213, 43);
            this->fcgCXMKVCmdEx->Name = L"fcgCXMKVCmdEx";
            this->fcgCXMKVCmdEx->Size = System::Drawing::Size(157, 22);
            this->fcgCXMKVCmdEx->TabIndex = 1;
            this->fcgCXMKVCmdEx->Tag = L"chValue";
            // 
            // fcgLBMKVMuxerCmdEx
            // 
            this->fcgLBMKVMuxerCmdEx->AutoSize = true;
            this->fcgLBMKVMuxerCmdEx->Location = System::Drawing::Point(139, 46);
            this->fcgLBMKVMuxerCmdEx->Name = L"fcgLBMKVMuxerCmdEx";
            this->fcgLBMKVMuxerCmdEx->Size = System::Drawing::Size(68, 14);
            this->fcgLBMKVMuxerCmdEx->TabIndex = 17;
            this->fcgLBMKVMuxerCmdEx->Text = L"拡張オプション";
            // 
            // fcgCBMKVMuxerExt
            // 
            this->fcgCBMKVMuxerExt->AutoSize = true;
            this->fcgCBMKVMuxerExt->Location = System::Drawing::Point(10, 45);
            this->fcgCBMKVMuxerExt->Name = L"fcgCBMKVMuxerExt";
            this->fcgCBMKVMuxerExt->Size = System::Drawing::Size(113, 18);
            this->fcgCBMKVMuxerExt->TabIndex = 0;
            this->fcgCBMKVMuxerExt->Tag = L"chValue";
            this->fcgCBMKVMuxerExt->Text = L"外部muxerを使用";
            this->fcgCBMKVMuxerExt->UseVisualStyleBackColor = true;
            // 
            // fcgtabPageMux
            // 
            this->fcgtabPageMux->Controls->Add(this->fcgCXMuxPriority);
            this->fcgtabPageMux->Controls->Add(this->fcgLBMuxPriority);
            this->fcgtabPageMux->Location = System::Drawing::Point(4, 23);
            this->fcgtabPageMux->Name = L"fcgtabPageMux";
            this->fcgtabPageMux->Size = System::Drawing::Size(376, 187);
            this->fcgtabPageMux->TabIndex = 2;
            this->fcgtabPageMux->Text = L"Mux共通設定";
            this->fcgtabPageMux->UseVisualStyleBackColor = true;
            // 
            // fcgCXMuxPriority
            // 
            this->fcgCXMuxPriority->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXMuxPriority->FormattingEnabled = true;
            this->fcgCXMuxPriority->Location = System::Drawing::Point(102, 64);
            this->fcgCXMuxPriority->Name = L"fcgCXMuxPriority";
            this->fcgCXMuxPriority->Size = System::Drawing::Size(198, 22);
            this->fcgCXMuxPriority->TabIndex = 1;
            this->fcgCXMuxPriority->Tag = L"chValue";
            // 
            // fcgLBMuxPriority
            // 
            this->fcgLBMuxPriority->AutoSize = true;
            this->fcgLBMuxPriority->Location = System::Drawing::Point(15, 67);
            this->fcgLBMuxPriority->Name = L"fcgLBMuxPriority";
            this->fcgLBMuxPriority->Size = System::Drawing::Size(62, 14);
            this->fcgLBMuxPriority->TabIndex = 1;
            this->fcgLBMuxPriority->Text = L"Mux優先度";
            // 
            // fcgtabPageBat
            // 
            this->fcgtabPageBat->Controls->Add(this->fcgLBBatAfterString);
            this->fcgtabPageBat->Controls->Add(this->fcgLBBatBeforeString);
            this->fcgtabPageBat->Controls->Add(this->fcgBTBatAfterPath);
            this->fcgtabPageBat->Controls->Add(this->fcgTXBatAfterPath);
            this->fcgtabPageBat->Controls->Add(this->fcgLBBatAfterPath);
            this->fcgtabPageBat->Controls->Add(this->fcgCBWaitForBatAfter);
            this->fcgtabPageBat->Controls->Add(this->fcgCBRunBatAfter);
            this->fcgtabPageBat->Controls->Add(this->fcgPNSeparator);
            this->fcgtabPageBat->Controls->Add(this->fcgBTBatBeforePath);
            this->fcgtabPageBat->Controls->Add(this->fcgTXBatBeforePath);
            this->fcgtabPageBat->Controls->Add(this->fcgLBBatBeforePath);
            this->fcgtabPageBat->Controls->Add(this->fcgCBWaitForBatBefore);
            this->fcgtabPageBat->Controls->Add(this->fcgCBRunBatBefore);
            this->fcgtabPageBat->Location = System::Drawing::Point(4, 23);
            this->fcgtabPageBat->Name = L"fcgtabPageBat";
            this->fcgtabPageBat->Size = System::Drawing::Size(376, 187);
            this->fcgtabPageBat->TabIndex = 3;
            this->fcgtabPageBat->Text = L"エンコ前後バッチ処理";
            this->fcgtabPageBat->UseVisualStyleBackColor = true;
            // 
            // fcgLBBatAfterString
            // 
            this->fcgLBBatAfterString->AutoSize = true;
            this->fcgLBBatAfterString->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 9, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Italic | System::Drawing::FontStyle::Underline)),
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
            this->fcgLBBatAfterString->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
            this->fcgLBBatAfterString->Location = System::Drawing::Point(304, 113);
            this->fcgLBBatAfterString->Name = L"fcgLBBatAfterString";
            this->fcgLBBatAfterString->Size = System::Drawing::Size(27, 15);
            this->fcgLBBatAfterString->TabIndex = 13;
            this->fcgLBBatAfterString->Text = L" 後& ";
            this->fcgLBBatAfterString->TextAlign = System::Drawing::ContentAlignment::TopCenter;
            // 
            // fcgLBBatBeforeString
            // 
            this->fcgLBBatBeforeString->AutoSize = true;
            this->fcgLBBatBeforeString->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 9, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Italic | System::Drawing::FontStyle::Underline)),
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
            this->fcgLBBatBeforeString->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
            this->fcgLBBatBeforeString->Location = System::Drawing::Point(304, 20);
            this->fcgLBBatBeforeString->Name = L"fcgLBBatBeforeString";
            this->fcgLBBatBeforeString->Size = System::Drawing::Size(27, 15);
            this->fcgLBBatBeforeString->TabIndex = 3;
            this->fcgLBBatBeforeString->Text = L" 前& ";
            this->fcgLBBatBeforeString->TextAlign = System::Drawing::ContentAlignment::TopCenter;
            // 
            // fcgBTBatAfterPath
            // 
            this->fcgBTBatAfterPath->Location = System::Drawing::Point(330, 154);
            this->fcgBTBatAfterPath->Name = L"fcgBTBatAfterPath";
            this->fcgBTBatAfterPath->Size = System::Drawing::Size(30, 23);
            this->fcgBTBatAfterPath->TabIndex = 11;
            this->fcgBTBatAfterPath->Tag = L"chValue";
            this->fcgBTBatAfterPath->Text = L"...";
            this->fcgBTBatAfterPath->UseVisualStyleBackColor = true;
            this->fcgBTBatAfterPath->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTBatAfterPath_Click);
            // 
            // fcgTXBatAfterPath
            // 
            this->fcgTXBatAfterPath->AllowDrop = true;
            this->fcgTXBatAfterPath->Location = System::Drawing::Point(126, 155);
            this->fcgTXBatAfterPath->Name = L"fcgTXBatAfterPath";
            this->fcgTXBatAfterPath->Size = System::Drawing::Size(202, 21);
            this->fcgTXBatAfterPath->TabIndex = 10;
            this->fcgTXBatAfterPath->Tag = L"chValue";
            this->fcgTXBatAfterPath->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXBatAfterPath->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            // 
            // fcgLBBatAfterPath
            // 
            this->fcgLBBatAfterPath->AutoSize = true;
            this->fcgLBBatAfterPath->Location = System::Drawing::Point(40, 158);
            this->fcgLBBatAfterPath->Name = L"fcgLBBatAfterPath";
            this->fcgLBBatAfterPath->Size = System::Drawing::Size(0, 14);
            this->fcgLBBatAfterPath->TabIndex = 9;
            // 
            // fcgCBWaitForBatAfter
            // 
            this->fcgCBWaitForBatAfter->AutoSize = true;
            this->fcgCBWaitForBatAfter->Location = System::Drawing::Point(40, 129);
            this->fcgCBWaitForBatAfter->Name = L"fcgCBWaitForBatAfter";
            this->fcgCBWaitForBatAfter->Size = System::Drawing::Size(150, 18);
            this->fcgCBWaitForBatAfter->TabIndex = 8;
            this->fcgCBWaitForBatAfter->Tag = L"chValue";
            this->fcgCBWaitForBatAfter->Text = L"バッチ処理の終了を待機する";
            this->fcgCBWaitForBatAfter->UseVisualStyleBackColor = true;
            // 
            // fcgCBRunBatAfter
            // 
            this->fcgCBRunBatAfter->AutoSize = true;
            this->fcgCBRunBatAfter->Location = System::Drawing::Point(18, 105);
            this->fcgCBRunBatAfter->Name = L"fcgCBRunBatAfter";
            this->fcgCBRunBatAfter->Size = System::Drawing::Size(179, 18);
            this->fcgCBRunBatAfter->TabIndex = 7;
            this->fcgCBRunBatAfter->Tag = L"chValue";
            this->fcgCBRunBatAfter->Text = L"エンコード終了後、バッチ処理を行う";
            this->fcgCBRunBatAfter->UseVisualStyleBackColor = true;
            // 
            // fcgPNSeparator
            // 
            this->fcgPNSeparator->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->fcgPNSeparator->Location = System::Drawing::Point(18, 94);
            this->fcgPNSeparator->Name = L"fcgPNSeparator";
            this->fcgPNSeparator->Size = System::Drawing::Size(342, 1);
            this->fcgPNSeparator->TabIndex = 6;
            // 
            // fcgBTBatBeforePath
            // 
            this->fcgBTBatBeforePath->Location = System::Drawing::Point(330, 61);
            this->fcgBTBatBeforePath->Name = L"fcgBTBatBeforePath";
            this->fcgBTBatBeforePath->Size = System::Drawing::Size(30, 23);
            this->fcgBTBatBeforePath->TabIndex = 5;
            this->fcgBTBatBeforePath->Tag = L"chValue";
            this->fcgBTBatBeforePath->Text = L"...";
            this->fcgBTBatBeforePath->UseVisualStyleBackColor = true;
            this->fcgBTBatBeforePath->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTBatBeforePath_Click);
            // 
            // fcgTXBatBeforePath
            // 
            this->fcgTXBatBeforePath->AllowDrop = true;
            this->fcgTXBatBeforePath->Location = System::Drawing::Point(126, 62);
            this->fcgTXBatBeforePath->Name = L"fcgTXBatBeforePath";
            this->fcgTXBatBeforePath->Size = System::Drawing::Size(202, 21);
            this->fcgTXBatBeforePath->TabIndex = 4;
            this->fcgTXBatBeforePath->Tag = L"chValue";
            this->fcgTXBatBeforePath->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXBatBeforePath->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            // 
            // fcgLBBatBeforePath
            // 
            this->fcgLBBatBeforePath->AutoSize = true;
            this->fcgLBBatBeforePath->Location = System::Drawing::Point(40, 65);
            this->fcgLBBatBeforePath->Name = L"fcgLBBatBeforePath";
            this->fcgLBBatBeforePath->Size = System::Drawing::Size(0, 14);
            this->fcgLBBatBeforePath->TabIndex = 2;
            // 
            // fcgCBWaitForBatBefore
            // 
            this->fcgCBWaitForBatBefore->AutoSize = true;
            this->fcgCBWaitForBatBefore->Location = System::Drawing::Point(40, 36);
            this->fcgCBWaitForBatBefore->Name = L"fcgCBWaitForBatBefore";
            this->fcgCBWaitForBatBefore->Size = System::Drawing::Size(150, 18);
            this->fcgCBWaitForBatBefore->TabIndex = 1;
            this->fcgCBWaitForBatBefore->Tag = L"chValue";
            this->fcgCBWaitForBatBefore->Text = L"バッチ処理の終了を待機する";
            this->fcgCBWaitForBatBefore->UseVisualStyleBackColor = true;
            // 
            // fcgCBRunBatBefore
            // 
            this->fcgCBRunBatBefore->AutoSize = true;
            this->fcgCBRunBatBefore->Location = System::Drawing::Point(18, 12);
            this->fcgCBRunBatBefore->Name = L"fcgCBRunBatBefore";
            this->fcgCBRunBatBefore->Size = System::Drawing::Size(179, 18);
            this->fcgCBRunBatBefore->TabIndex = 0;
            this->fcgCBRunBatBefore->Tag = L"chValue";
            this->fcgCBRunBatBefore->Text = L"エンコード開始前、バッチ処理を行う";
            this->fcgCBRunBatBefore->UseVisualStyleBackColor = true;
            // 
            // fcgTXCmd
            // 
            this->fcgTXCmd->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
                | System::Windows::Forms::AnchorStyles::Left)
                | System::Windows::Forms::AnchorStyles::Right));
            this->fcgTXCmd->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->fcgTXCmd->Location = System::Drawing::Point(9, 547);
            this->fcgTXCmd->Name = L"fcgTXCmd";
            this->fcgTXCmd->ReadOnly = true;
            this->fcgTXCmd->Size = System::Drawing::Size(992, 21);
            this->fcgTXCmd->TabIndex = 4;
            this->fcgTXCmd->DoubleClick += gcnew System::EventHandler(this, &frmConfig::fcgTXCmd_DoubleClick);
            // 
            // fcgBTCancel
            // 
            this->fcgBTCancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
            this->fcgBTCancel->Location = System::Drawing::Point(771, 570);
            this->fcgBTCancel->Name = L"fcgBTCancel";
            this->fcgBTCancel->Size = System::Drawing::Size(84, 28);
            this->fcgBTCancel->TabIndex = 5;
            this->fcgBTCancel->Text = L"キャンセル";
            this->fcgBTCancel->UseVisualStyleBackColor = true;
            this->fcgBTCancel->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTCancel_Click);
            // 
            // fcgBTOK
            // 
            this->fcgBTOK->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
            this->fcgBTOK->Location = System::Drawing::Point(893, 570);
            this->fcgBTOK->Name = L"fcgBTOK";
            this->fcgBTOK->Size = System::Drawing::Size(84, 28);
            this->fcgBTOK->TabIndex = 6;
            this->fcgBTOK->Text = L"OK";
            this->fcgBTOK->UseVisualStyleBackColor = true;
            this->fcgBTOK->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTOK_Click);
            // 
            // fcgBTDefault
            // 
            this->fcgBTDefault->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
            this->fcgBTDefault->Location = System::Drawing::Point(9, 572);
            this->fcgBTDefault->Name = L"fcgBTDefault";
            this->fcgBTDefault->Size = System::Drawing::Size(112, 28);
            this->fcgBTDefault->TabIndex = 7;
            this->fcgBTDefault->Text = L"デフォルト";
            this->fcgBTDefault->UseVisualStyleBackColor = true;
            this->fcgBTDefault->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTDefault_Click);
            // 
            // fcgLBVersionDate
            // 
            this->fcgLBVersionDate->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
            this->fcgLBVersionDate->AutoSize = true;
            this->fcgLBVersionDate->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 9, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->fcgLBVersionDate->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
            this->fcgLBVersionDate->Location = System::Drawing::Point(416, 579);
            this->fcgLBVersionDate->Name = L"fcgLBVersionDate";
            this->fcgLBVersionDate->Size = System::Drawing::Size(49, 15);
            this->fcgLBVersionDate->TabIndex = 8;
            this->fcgLBVersionDate->Text = L"Version";
            // 
            // fcgLBVersion
            // 
            this->fcgLBVersion->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
            this->fcgLBVersion->AutoSize = true;
            this->fcgLBVersion->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 9, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->fcgLBVersion->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
            this->fcgLBVersion->Location = System::Drawing::Point(180, 579);
            this->fcgLBVersion->Name = L"fcgLBVersion";
            this->fcgLBVersion->Size = System::Drawing::Size(49, 15);
            this->fcgLBVersion->TabIndex = 9;
            this->fcgLBVersion->Text = L"Version";
            // 
            // fcgOpenFileDialog
            // 
            this->fcgOpenFileDialog->FileName = L"openFileDialog1";
            // 
            // fcgTTEx
            // 
            this->fcgTTEx->AutomaticDelay = 200;
            this->fcgTTEx->AutoPopDelay = 9999;
            this->fcgTTEx->InitialDelay = 200;
            this->fcgTTEx->IsBalloon = true;
            this->fcgTTEx->ReshowDelay = 50;
            this->fcgTTEx->ShowAlways = true;
            this->fcgTTEx->UseAnimation = false;
            this->fcgTTEx->UseFading = false;
            // 
            // fcgCSReplaceStrings
            // 
            this->fcgCSReplaceStrings->ImageScalingSize = System::Drawing::Size(18, 18);
            this->fcgCSReplaceStrings->Name = L"fcgCSReplaceStrings";
            this->fcgCSReplaceStrings->Size = System::Drawing::Size(61, 4);
            // 
            // fcgLBguiExBlog
            // 
            this->fcgLBguiExBlog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
            this->fcgLBguiExBlog->AutoSize = true;
            this->fcgLBguiExBlog->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 8.25F, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->fcgLBguiExBlog->LinkColor = System::Drawing::Color::Gray;
            this->fcgLBguiExBlog->Location = System::Drawing::Point(623, 579);
            this->fcgLBguiExBlog->Name = L"fcgLBguiExBlog";
            this->fcgLBguiExBlog->Size = System::Drawing::Size(100, 14);
            this->fcgLBguiExBlog->TabIndex = 10;
            this->fcgLBguiExBlog->TabStop = true;
            this->fcgLBguiExBlog->Text = L"x265guiExについて";
            this->fcgLBguiExBlog->VisitedLinkColor = System::Drawing::Color::Gray;
            this->fcgLBguiExBlog->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &frmConfig::fcgLBguiExBlog_LinkClicked);
            // 
            // fcgTTX265Version
            // 
            this->fcgTTX265Version->AutomaticDelay = 200;
            this->fcgTTX265Version->AutoPopDelay = 10000;
            this->fcgTTX265Version->InitialDelay = 200;
            this->fcgTTX265Version->IsBalloon = true;
            this->fcgTTX265Version->ReshowDelay = 50;
            this->fcgTTX265Version->ToolTipTitle = L"x265 バージョン情報...";
            this->fcgTTX265Version->UseAnimation = false;
            this->fcgTTX265Version->UseFading = false;
            // 
            // fcgTTX265
            // 
            this->fcgTTX265->AutomaticDelay = 200;
            this->fcgTTX265->AutoPopDelay = 9999;
            this->fcgTTX265->InitialDelay = 200;
            this->fcgTTX265->IsBalloon = true;
            this->fcgTTX265->ReshowDelay = 40;
            this->fcgTTX265->ShowAlways = true;
            this->fcgTTX265->ToolTipTitle = L"x265 options...";
            this->fcgTTX265->UseAnimation = false;
            this->fcgTTX265->UseFading = false;
            // 
            // fcgCSExeFiles
            // 
            this->fcgCSExeFiles->ImageScalingSize = System::Drawing::Size(18, 18);
            this->fcgCSExeFiles->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fcgTSExeFileshelp });
            this->fcgCSExeFiles->Name = L"fcgCSx264";
            this->fcgCSExeFiles->Size = System::Drawing::Size(131, 26);
            // 
            // fcgTSExeFileshelp
            // 
            this->fcgTSExeFileshelp->Name = L"fcgTSExeFileshelp";
            this->fcgTSExeFileshelp->Size = System::Drawing::Size(130, 22);
            this->fcgTSExeFileshelp->Text = L"helpを表示";
            this->fcgTSExeFileshelp->Click += gcnew System::EventHandler(this, &frmConfig::fcgTSExeFileshelp_Click);
            // 
            // fcgtabControlAudio
            // 
            this->fcgtabControlAudio->Controls->Add(this->fcgtabPageAudioMain);
            this->fcgtabControlAudio->Controls->Add(this->fcgtabPageAudioOther);
            this->fcgtabControlAudio->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 8.25F));
            this->fcgtabControlAudio->Location = System::Drawing::Point(2, 2);
            this->fcgtabControlAudio->Name = L"fcgtabControlAudio";
            this->fcgtabControlAudio->SelectedIndex = 0;
            this->fcgtabControlAudio->Size = System::Drawing::Size(384, 296);
            this->fcgtabControlAudio->TabIndex = 12;
            // 
            // fcgtabPageAudioMain
            // 
            this->fcgtabPageAudioMain->Controls->Add(this->fcgCXAudioDelayCut);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgLBAudioDelayCut);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgCBAudioEncTiming);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgCXAudioEncTiming);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgCXAudioTempDir);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgTXCustomAudioTempDir);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgBTCustomAudioTempDir);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgCBAudioUsePipe);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgLBAudioBitrate);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgNUAudioBitrate);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgCBAudio2pass);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgCXAudioEncMode);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgLBAudioEncMode);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgBTAudioEncoderPath);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgTXAudioEncoderPath);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgLBAudioEncoderPath);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgCBAudioOnly);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgCBFAWCheck);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgCXAudioEncoder);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgLBAudioEncoder);
            this->fcgtabPageAudioMain->Controls->Add(this->fcgLBAudioTemp);
            this->fcgtabPageAudioMain->Location = System::Drawing::Point(4, 23);
            this->fcgtabPageAudioMain->Name = L"fcgtabPageAudioMain";
            this->fcgtabPageAudioMain->Padding = System::Windows::Forms::Padding(3);
            this->fcgtabPageAudioMain->Size = System::Drawing::Size(376, 269);
            this->fcgtabPageAudioMain->TabIndex = 0;
            this->fcgtabPageAudioMain->Text = L"音声";
            this->fcgtabPageAudioMain->UseVisualStyleBackColor = true;
            // 
            // fcgCXAudioDelayCut
            // 
            this->fcgCXAudioDelayCut->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXAudioDelayCut->FormattingEnabled = true;
            this->fcgCXAudioDelayCut->Location = System::Drawing::Point(291, 133);
            this->fcgCXAudioDelayCut->Name = L"fcgCXAudioDelayCut";
            this->fcgCXAudioDelayCut->Size = System::Drawing::Size(70, 22);
            this->fcgCXAudioDelayCut->TabIndex = 54;
            this->fcgCXAudioDelayCut->Tag = L"chValue";
            // 
            // fcgLBAudioDelayCut
            // 
            this->fcgLBAudioDelayCut->AutoSize = true;
            this->fcgLBAudioDelayCut->Location = System::Drawing::Point(218, 136);
            this->fcgLBAudioDelayCut->Name = L"fcgLBAudioDelayCut";
            this->fcgLBAudioDelayCut->Size = System::Drawing::Size(60, 14);
            this->fcgLBAudioDelayCut->TabIndex = 54;
            this->fcgLBAudioDelayCut->Text = L"ディレイカット";
            // 
            // fcgCBAudioEncTiming
            // 
            this->fcgCBAudioEncTiming->AutoSize = true;
            this->fcgCBAudioEncTiming->Location = System::Drawing::Point(234, 54);
            this->fcgCBAudioEncTiming->Name = L"fcgCBAudioEncTiming";
            this->fcgCBAudioEncTiming->Size = System::Drawing::Size(40, 14);
            this->fcgCBAudioEncTiming->TabIndex = 53;
            this->fcgCBAudioEncTiming->Text = L"処理順";
            // 
            // fcgCXAudioEncTiming
            // 
            this->fcgCXAudioEncTiming->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXAudioEncTiming->FormattingEnabled = true;
            this->fcgCXAudioEncTiming->Location = System::Drawing::Point(286, 51);
            this->fcgCXAudioEncTiming->Name = L"fcgCXAudioEncTiming";
            this->fcgCXAudioEncTiming->Size = System::Drawing::Size(68, 22);
            this->fcgCXAudioEncTiming->TabIndex = 37;
            this->fcgCXAudioEncTiming->Tag = L"chValue";
            // 
            // fcgCXAudioTempDir
            // 
            this->fcgCXAudioTempDir->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXAudioTempDir->FormattingEnabled = true;
            this->fcgCXAudioTempDir->Location = System::Drawing::Point(135, 208);
            this->fcgCXAudioTempDir->Name = L"fcgCXAudioTempDir";
            this->fcgCXAudioTempDir->Size = System::Drawing::Size(150, 22);
            this->fcgCXAudioTempDir->TabIndex = 60;
            this->fcgCXAudioTempDir->Tag = L"chValue";
            // 
            // fcgTXCustomAudioTempDir
            // 
            this->fcgTXCustomAudioTempDir->Location = System::Drawing::Point(64, 236);
            this->fcgTXCustomAudioTempDir->Name = L"fcgTXCustomAudioTempDir";
            this->fcgTXCustomAudioTempDir->Size = System::Drawing::Size(245, 21);
            this->fcgTXCustomAudioTempDir->TabIndex = 61;
            this->fcgTXCustomAudioTempDir->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTXCustomAudioTempDir_TextChanged);
            // 
            // fcgBTCustomAudioTempDir
            // 
            this->fcgBTCustomAudioTempDir->Location = System::Drawing::Point(316, 234);
            this->fcgBTCustomAudioTempDir->Name = L"fcgBTCustomAudioTempDir";
            this->fcgBTCustomAudioTempDir->Size = System::Drawing::Size(29, 23);
            this->fcgBTCustomAudioTempDir->TabIndex = 62;
            this->fcgBTCustomAudioTempDir->Text = L"...";
            this->fcgBTCustomAudioTempDir->UseVisualStyleBackColor = true;
            this->fcgBTCustomAudioTempDir->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTCustomAudioTempDir_Click);
            // 
            // fcgCBAudioUsePipe
            // 
            this->fcgCBAudioUsePipe->AutoSize = true;
            this->fcgCBAudioUsePipe->Location = System::Drawing::Point(126, 134);
            this->fcgCBAudioUsePipe->Name = L"fcgCBAudioUsePipe";
            this->fcgCBAudioUsePipe->Size = System::Drawing::Size(73, 18);
            this->fcgCBAudioUsePipe->TabIndex = 53;
            this->fcgCBAudioUsePipe->Tag = L"chValue";
            this->fcgCBAudioUsePipe->Text = L"パイプ処理";
            this->fcgCBAudioUsePipe->UseVisualStyleBackColor = true;
            // 
            // fcgLBAudioBitrate
            // 
            this->fcgLBAudioBitrate->AutoSize = true;
            this->fcgLBAudioBitrate->Location = System::Drawing::Point(284, 161);
            this->fcgLBAudioBitrate->Name = L"fcgLBAudioBitrate";
            this->fcgLBAudioBitrate->Size = System::Drawing::Size(32, 14);
            this->fcgLBAudioBitrate->TabIndex = 50;
            this->fcgLBAudioBitrate->Text = L"kbps";
            // 
            // fcgNUAudioBitrate
            // 
            this->fcgNUAudioBitrate->Location = System::Drawing::Point(212, 157);
            this->fcgNUAudioBitrate->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1536, 0, 0, 0 });
            this->fcgNUAudioBitrate->Name = L"fcgNUAudioBitrate";
            this->fcgNUAudioBitrate->Size = System::Drawing::Size(65, 21);
            this->fcgNUAudioBitrate->TabIndex = 51;
            this->fcgNUAudioBitrate->Tag = L"chValue";
            this->fcgNUAudioBitrate->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
            // 
            // fcgCBAudio2pass
            // 
            this->fcgCBAudio2pass->AutoSize = true;
            this->fcgCBAudio2pass->Location = System::Drawing::Point(55, 134);
            this->fcgCBAudio2pass->Name = L"fcgCBAudio2pass";
            this->fcgCBAudio2pass->Size = System::Drawing::Size(56, 18);
            this->fcgCBAudio2pass->TabIndex = 52;
            this->fcgCBAudio2pass->Tag = L"chValue";
            this->fcgCBAudio2pass->Text = L"2pass";
            this->fcgCBAudio2pass->UseVisualStyleBackColor = true;
            this->fcgCBAudio2pass->CheckedChanged += gcnew System::EventHandler(this, &frmConfig::fcgCBAudio2pass_CheckedChanged);
            // 
            // fcgCXAudioEncMode
            // 
            this->fcgCXAudioEncMode->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXAudioEncMode->FormattingEnabled = true;
            this->fcgCXAudioEncMode->Location = System::Drawing::Point(16, 156);
            this->fcgCXAudioEncMode->Name = L"fcgCXAudioEncMode";
            this->fcgCXAudioEncMode->Size = System::Drawing::Size(189, 22);
            this->fcgCXAudioEncMode->TabIndex = 50;
            this->fcgCXAudioEncMode->Tag = L"chValue";
            this->fcgCXAudioEncMode->SelectedIndexChanged += gcnew System::EventHandler(this, &frmConfig::fcgCXAudioEncMode_SelectedIndexChanged);
            // 
            // fcgLBAudioEncMode
            // 
            this->fcgLBAudioEncMode->AutoSize = true;
            this->fcgLBAudioEncMode->Location = System::Drawing::Point(4, 136);
            this->fcgLBAudioEncMode->Name = L"fcgLBAudioEncMode";
            this->fcgLBAudioEncMode->Size = System::Drawing::Size(32, 14);
            this->fcgLBAudioEncMode->TabIndex = 48;
            this->fcgLBAudioEncMode->Text = L"モード";
            // 
            // fcgBTAudioEncoderPath
            // 
            this->fcgBTAudioEncoderPath->Location = System::Drawing::Point(324, 90);
            this->fcgBTAudioEncoderPath->Name = L"fcgBTAudioEncoderPath";
            this->fcgBTAudioEncoderPath->Size = System::Drawing::Size(30, 23);
            this->fcgBTAudioEncoderPath->TabIndex = 41;
            this->fcgBTAudioEncoderPath->Text = L"...";
            this->fcgBTAudioEncoderPath->UseVisualStyleBackColor = true;
            this->fcgBTAudioEncoderPath->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTAudioEncoderPath_Click);
            // 
            // fcgTXAudioEncoderPath
            // 
            this->fcgTXAudioEncoderPath->AllowDrop = true;
            this->fcgTXAudioEncoderPath->Location = System::Drawing::Point(16, 92);
            this->fcgTXAudioEncoderPath->Name = L"fcgTXAudioEncoderPath";
            this->fcgTXAudioEncoderPath->Size = System::Drawing::Size(303, 21);
            this->fcgTXAudioEncoderPath->TabIndex = 40;
            this->fcgTXAudioEncoderPath->TextChanged += gcnew System::EventHandler(this, &frmConfig::fcgTXAudioEncoderPath_TextChanged);
            this->fcgTXAudioEncoderPath->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXAudioEncoderPath->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            this->fcgTXAudioEncoderPath->Enter += gcnew System::EventHandler(this, &frmConfig::fcgTXAudioEncoderPath_Enter);
            this->fcgTXAudioEncoderPath->Leave += gcnew System::EventHandler(this, &frmConfig::fcgTXAudioEncoderPath_Leave);
            // 
            // fcgLBAudioEncoderPath
            // 
            this->fcgLBAudioEncoderPath->AutoSize = true;
            this->fcgLBAudioEncoderPath->Location = System::Drawing::Point(12, 75);
            this->fcgLBAudioEncoderPath->Name = L"fcgLBAudioEncoderPath";
            this->fcgLBAudioEncoderPath->Size = System::Drawing::Size(49, 14);
            this->fcgLBAudioEncoderPath->TabIndex = 44;
            this->fcgLBAudioEncoderPath->Text = L"～の指定";
            // 
            // fcgCBAudioOnly
            // 
            this->fcgCBAudioOnly->AutoSize = true;
            this->fcgCBAudioOnly->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
            this->fcgCBAudioOnly->Location = System::Drawing::Point(252, 5);
            this->fcgCBAudioOnly->Name = L"fcgCBAudioOnly";
            this->fcgCBAudioOnly->Size = System::Drawing::Size(89, 18);
            this->fcgCBAudioOnly->TabIndex = 34;
            this->fcgCBAudioOnly->Tag = L"chValue";
            this->fcgCBAudioOnly->Text = L"音声のみ出力";
            this->fcgCBAudioOnly->UseVisualStyleBackColor = true;
            // 
            // fcgCBFAWCheck
            // 
            this->fcgCBFAWCheck->AutoSize = true;
            this->fcgCBFAWCheck->Location = System::Drawing::Point(252, 28);
            this->fcgCBFAWCheck->Name = L"fcgCBFAWCheck";
            this->fcgCBFAWCheck->Size = System::Drawing::Size(81, 18);
            this->fcgCBFAWCheck->TabIndex = 36;
            this->fcgCBFAWCheck->Tag = L"chValue";
            this->fcgCBFAWCheck->Text = L"FAWCheck";
            this->fcgCBFAWCheck->UseVisualStyleBackColor = true;
            // 
            // fcgCXAudioEncoder
            // 
            this->fcgCXAudioEncoder->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXAudioEncoder->FormattingEnabled = true;
            this->fcgCXAudioEncoder->Location = System::Drawing::Point(17, 34);
            this->fcgCXAudioEncoder->Name = L"fcgCXAudioEncoder";
            this->fcgCXAudioEncoder->Size = System::Drawing::Size(172, 22);
            this->fcgCXAudioEncoder->TabIndex = 32;
            this->fcgCXAudioEncoder->Tag = L"chValue";
            this->fcgCXAudioEncoder->SelectedIndexChanged += gcnew System::EventHandler(this, &frmConfig::fcgCXAudioEncoder_SelectedIndexChanged);
            // 
            // fcgLBAudioEncoder
            // 
            this->fcgLBAudioEncoder->AutoSize = true;
            this->fcgLBAudioEncoder->Location = System::Drawing::Point(5, 14);
            this->fcgLBAudioEncoder->Name = L"fcgLBAudioEncoder";
            this->fcgLBAudioEncoder->Size = System::Drawing::Size(48, 14);
            this->fcgLBAudioEncoder->TabIndex = 33;
            this->fcgLBAudioEncoder->Text = L"エンコーダ";
            // 
            // fcgLBAudioTemp
            // 
            this->fcgLBAudioTemp->AutoSize = true;
            this->fcgLBAudioTemp->Location = System::Drawing::Point(7, 211);
            this->fcgLBAudioTemp->Name = L"fcgLBAudioTemp";
            this->fcgLBAudioTemp->Size = System::Drawing::Size(114, 14);
            this->fcgLBAudioTemp->TabIndex = 51;
            this->fcgLBAudioTemp->Text = L"音声一時ファイル出力先";
            // 
            // fcgtabPageAudioOther
            // 
            this->fcgtabPageAudioOther->Controls->Add(this->panel2);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgLBBatAfterAudioString);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgLBBatBeforeAudioString);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgBTBatAfterAudioPath);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgTXBatAfterAudioPath);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgLBBatAfterAudioPath);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgCBRunBatAfterAudio);
            this->fcgtabPageAudioOther->Controls->Add(this->panel1);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgBTBatBeforeAudioPath);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgTXBatBeforeAudioPath);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgLBBatBeforeAudioPath);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgCBRunBatBeforeAudio);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgCXAudioPriority);
            this->fcgtabPageAudioOther->Controls->Add(this->fcgLBAudioPriority);
            this->fcgtabPageAudioOther->Location = System::Drawing::Point(4, 23);
            this->fcgtabPageAudioOther->Name = L"fcgtabPageAudioOther";
            this->fcgtabPageAudioOther->Padding = System::Windows::Forms::Padding(3);
            this->fcgtabPageAudioOther->Size = System::Drawing::Size(376, 269);
            this->fcgtabPageAudioOther->TabIndex = 1;
            this->fcgtabPageAudioOther->Text = L"その他";
            this->fcgtabPageAudioOther->UseVisualStyleBackColor = true;
            // 
            // panel2
            // 
            this->panel2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->panel2->Location = System::Drawing::Point(18, 126);
            this->panel2->Name = L"panel2";
            this->panel2->Size = System::Drawing::Size(342, 1);
            this->panel2->TabIndex = 61;
            // 
            // fcgLBBatAfterAudioString
            // 
            this->fcgLBBatAfterAudioString->AutoSize = true;
            this->fcgLBBatAfterAudioString->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 9, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Italic | System::Drawing::FontStyle::Underline)),
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
            this->fcgLBBatAfterAudioString->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
            this->fcgLBBatAfterAudioString->Location = System::Drawing::Point(304, 208);
            this->fcgLBBatAfterAudioString->Name = L"fcgLBBatAfterAudioString";
            this->fcgLBBatAfterAudioString->Size = System::Drawing::Size(27, 15);
            this->fcgLBBatAfterAudioString->TabIndex = 60;
            this->fcgLBBatAfterAudioString->Text = L" 後& ";
            this->fcgLBBatAfterAudioString->TextAlign = System::Drawing::ContentAlignment::TopCenter;
            // 
            // fcgLBBatBeforeAudioString
            // 
            this->fcgLBBatBeforeAudioString->AutoSize = true;
            this->fcgLBBatBeforeAudioString->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 9, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Italic | System::Drawing::FontStyle::Underline)),
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(128)));
            this->fcgLBBatBeforeAudioString->ForeColor = System::Drawing::SystemColors::ControlDarkDark;
            this->fcgLBBatBeforeAudioString->Location = System::Drawing::Point(304, 139);
            this->fcgLBBatBeforeAudioString->Name = L"fcgLBBatBeforeAudioString";
            this->fcgLBBatBeforeAudioString->Size = System::Drawing::Size(27, 15);
            this->fcgLBBatBeforeAudioString->TabIndex = 51;
            this->fcgLBBatBeforeAudioString->Text = L" 前& ";
            this->fcgLBBatBeforeAudioString->TextAlign = System::Drawing::ContentAlignment::TopCenter;
            // 
            // fcgBTBatAfterAudioPath
            // 
            this->fcgBTBatAfterAudioPath->Location = System::Drawing::Point(330, 231);
            this->fcgBTBatAfterAudioPath->Name = L"fcgBTBatAfterAudioPath";
            this->fcgBTBatAfterAudioPath->Size = System::Drawing::Size(30, 23);
            this->fcgBTBatAfterAudioPath->TabIndex = 59;
            this->fcgBTBatAfterAudioPath->Tag = L"chValue";
            this->fcgBTBatAfterAudioPath->Text = L"...";
            this->fcgBTBatAfterAudioPath->UseVisualStyleBackColor = true;
            this->fcgBTBatAfterAudioPath->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTBatAfterAudioPath_Click);
            // 
            // fcgTXBatAfterAudioPath
            // 
            this->fcgTXBatAfterAudioPath->AllowDrop = true;
            this->fcgTXBatAfterAudioPath->Location = System::Drawing::Point(126, 232);
            this->fcgTXBatAfterAudioPath->Name = L"fcgTXBatAfterAudioPath";
            this->fcgTXBatAfterAudioPath->Size = System::Drawing::Size(202, 21);
            this->fcgTXBatAfterAudioPath->TabIndex = 58;
            this->fcgTXBatAfterAudioPath->Tag = L"chValue";
            this->fcgTXBatAfterAudioPath->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXBatAfterAudioPath->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            // 
            // fcgLBBatAfterAudioPath
            // 
            this->fcgLBBatAfterAudioPath->AutoSize = true;
            this->fcgLBBatAfterAudioPath->Location = System::Drawing::Point(40, 236);
            this->fcgLBBatAfterAudioPath->Name = L"fcgLBBatAfterAudioPath";
            this->fcgLBBatAfterAudioPath->Size = System::Drawing::Size(0, 14);
            this->fcgLBBatAfterAudioPath->TabIndex = 57;
            // 
            // fcgCBRunBatAfterAudio
            // 
            this->fcgCBRunBatAfterAudio->AutoSize = true;
            this->fcgCBRunBatAfterAudio->Location = System::Drawing::Point(18, 207);
            this->fcgCBRunBatAfterAudio->Name = L"fcgCBRunBatAfterAudio";
            this->fcgCBRunBatAfterAudio->Size = System::Drawing::Size(201, 18);
            this->fcgCBRunBatAfterAudio->TabIndex = 55;
            this->fcgCBRunBatAfterAudio->Tag = L"chValue";
            this->fcgCBRunBatAfterAudio->Text = L"音声エンコード終了後、バッチ処理を行う";
            this->fcgCBRunBatAfterAudio->UseVisualStyleBackColor = true;
            // 
            // panel1
            // 
            this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->panel1->Location = System::Drawing::Point(18, 196);
            this->panel1->Name = L"panel1";
            this->panel1->Size = System::Drawing::Size(342, 1);
            this->panel1->TabIndex = 54;
            // 
            // fcgBTBatBeforeAudioPath
            // 
            this->fcgBTBatBeforeAudioPath->Location = System::Drawing::Point(330, 164);
            this->fcgBTBatBeforeAudioPath->Name = L"fcgBTBatBeforeAudioPath";
            this->fcgBTBatBeforeAudioPath->Size = System::Drawing::Size(30, 23);
            this->fcgBTBatBeforeAudioPath->TabIndex = 53;
            this->fcgBTBatBeforeAudioPath->Tag = L"chValue";
            this->fcgBTBatBeforeAudioPath->Text = L"...";
            this->fcgBTBatBeforeAudioPath->UseVisualStyleBackColor = true;
            this->fcgBTBatBeforeAudioPath->Click += gcnew System::EventHandler(this, &frmConfig::fcgBTBatBeforeAudioPath_Click);
            // 
            // fcgTXBatBeforeAudioPath
            // 
            this->fcgTXBatBeforeAudioPath->AllowDrop = true;
            this->fcgTXBatBeforeAudioPath->Location = System::Drawing::Point(126, 164);
            this->fcgTXBatBeforeAudioPath->Name = L"fcgTXBatBeforeAudioPath";
            this->fcgTXBatBeforeAudioPath->Size = System::Drawing::Size(202, 21);
            this->fcgTXBatBeforeAudioPath->TabIndex = 52;
            this->fcgTXBatBeforeAudioPath->Tag = L"chValue";
            this->fcgTXBatBeforeAudioPath->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_DragDrop);
            this->fcgTXBatBeforeAudioPath->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &frmConfig::fcgSetDragDropFilename_Enter);
            // 
            // fcgLBBatBeforeAudioPath
            // 
            this->fcgLBBatBeforeAudioPath->AutoSize = true;
            this->fcgLBBatBeforeAudioPath->Location = System::Drawing::Point(40, 167);
            this->fcgLBBatBeforeAudioPath->Name = L"fcgLBBatBeforeAudioPath";
            this->fcgLBBatBeforeAudioPath->Size = System::Drawing::Size(0, 14);
            this->fcgLBBatBeforeAudioPath->TabIndex = 50;
            // 
            // fcgCBRunBatBeforeAudio
            // 
            this->fcgCBRunBatBeforeAudio->AutoSize = true;
            this->fcgCBRunBatBeforeAudio->Location = System::Drawing::Point(18, 139);
            this->fcgCBRunBatBeforeAudio->Name = L"fcgCBRunBatBeforeAudio";
            this->fcgCBRunBatBeforeAudio->Size = System::Drawing::Size(201, 18);
            this->fcgCBRunBatBeforeAudio->TabIndex = 48;
            this->fcgCBRunBatBeforeAudio->Tag = L"chValue";
            this->fcgCBRunBatBeforeAudio->Text = L"音声エンコード開始前、バッチ処理を行う";
            this->fcgCBRunBatBeforeAudio->UseVisualStyleBackColor = true;
            // 
            // fcgCXAudioPriority
            // 
            this->fcgCXAudioPriority->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fcgCXAudioPriority->FormattingEnabled = true;
            this->fcgCXAudioPriority->Location = System::Drawing::Point(156, 20);
            this->fcgCXAudioPriority->Name = L"fcgCXAudioPriority";
            this->fcgCXAudioPriority->Size = System::Drawing::Size(136, 22);
            this->fcgCXAudioPriority->TabIndex = 47;
            this->fcgCXAudioPriority->Tag = L"chValue";
            // 
            // fcgLBAudioPriority
            // 
            this->fcgLBAudioPriority->AutoSize = true;
            this->fcgLBAudioPriority->Location = System::Drawing::Point(29, 23);
            this->fcgLBAudioPriority->Name = L"fcgLBAudioPriority";
            this->fcgLBAudioPriority->Size = System::Drawing::Size(62, 14);
            this->fcgLBAudioPriority->TabIndex = 46;
            this->fcgLBAudioPriority->Text = L"音声優先度";
            // 
            // fcgPNHideTabControlMux
            // 
            this->fcgPNHideTabControlMux->Controls->Add(this->fcgtabControlMux);
            this->fcgPNHideTabControlMux->Location = System::Drawing::Point(620, 331);
            this->fcgPNHideTabControlMux->Name = L"fcgPNHideTabControlMux";
            this->fcgPNHideTabControlMux->Size = System::Drawing::Size(388, 218);
            this->fcgPNHideTabControlMux->TabIndex = 17;
            // 
            // fcgPNHideTabControlAudio
            // 
            this->fcgPNHideTabControlAudio->Controls->Add(this->fcgtabControlAudio);
            this->fcgPNHideTabControlAudio->Location = System::Drawing::Point(620, 25);
            this->fcgPNHideTabControlAudio->Name = L"fcgPNHideTabControlAudio";
            this->fcgPNHideTabControlAudio->Size = System::Drawing::Size(388, 300);
            this->fcgPNHideTabControlAudio->TabIndex = 16;
            // 
            // fcgPNHideTabControlVideo
            // 
            this->fcgPNHideTabControlVideo->Controls->Add(this->fcgtabControlVideo);
            this->fcgPNHideTabControlVideo->Location = System::Drawing::Point(-1, 25);
            this->fcgPNHideTabControlVideo->Name = L"fcgPNHideTabControlVideo";
            this->fcgPNHideTabControlVideo->Size = System::Drawing::Size(620, 524);
            this->fcgPNHideTabControlVideo->TabIndex = 15;
            // 
            // fcgPNHideToolStripBorder
            // 
            this->fcgPNHideToolStripBorder->Location = System::Drawing::Point(0, 22);
            this->fcgPNHideToolStripBorder->Name = L"fcgPNHideToolStripBorder";
            this->fcgPNHideToolStripBorder->Size = System::Drawing::Size(1020, 4);
            this->fcgPNHideToolStripBorder->TabIndex = 18;
            this->fcgPNHideToolStripBorder->Visible = false;
            // 
            // frmConfig
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(96, 96);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Dpi;
            this->ClientSize = System::Drawing::Size(1008, 601);
            this->Controls->Add(this->fcgPNHideToolStripBorder);
            this->Controls->Add(this->fcgPNHideTabControlAudio);
            this->Controls->Add(this->fcgPNHideTabControlMux);
            this->Controls->Add(this->fcgPNHideTabControlVideo);
            this->Controls->Add(this->fcgLBguiExBlog);
            this->Controls->Add(this->fcgLBVersion);
            this->Controls->Add(this->fcgLBVersionDate);
            this->Controls->Add(this->fcgBTDefault);
            this->Controls->Add(this->fcgBTOK);
            this->Controls->Add(this->fcgBTCancel);
            this->Controls->Add(this->fcgTXCmd);
            this->Controls->Add(this->fcgtoolStripSettings);
            this->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->MaximizeBox = false;
            this->Name = L"frmConfig";
            this->ShowIcon = false;
            this->Text = L"x265guiEx";
            this->Load += gcnew System::EventHandler(this, &frmConfig::frmConfig_Load);
            this->fcgtabControlVideo->ResumeLayout(false);
            this->fcgtabPageX265Main->ResumeLayout(false);
            this->fcgtabPageX265Main->PerformLayout();
            this->fcggroupBoxThread->ResumeLayout(false);
            this->fcggroupBoxThread->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUFrameThreads))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUPools))->EndInit();
            this->fcggroupBoxVUIColor->ResumeLayout(false);
            this->fcggroupBoxVUIColor->PerformLayout();
            this->fcgGroupBoxAspectRatio->ResumeLayout(false);
            this->fcgGroupBoxAspectRatio->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAspectRatioY))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAspectRatioX))->EndInit();
            this->fcggroupBoxRateControl->ResumeLayout(false);
            this->fcggroupBoxRateControl->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUQComp))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUVBVbuf))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUVBVmax))->EndInit();
            this->fcggroupBoxPreset->ResumeLayout(false);
            this->fcggroupBoxPreset->PerformLayout();
            this->fcgPNStatusFile->ResumeLayout(false);
            this->fcgPNStatusFile->PerformLayout();
            this->fcgPNBitrate->ResumeLayout(false);
            this->fcgPNBitrate->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgTBQuality))->EndInit();
            this->fcgPNX265Mode->ResumeLayout(false);
            this->fcgPNX265Mode->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAMPLimitBitrate))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAMPLimitFileSize))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAutoNPass))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgpictureBoxX265))->EndInit();
            this->fcgtabPageX265Other->ResumeLayout(false);
            this->fcgtabPageX265Other->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUQGSize))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURdoqLevel))->EndInit();
            this->fcggroupBoxDeblock->ResumeLayout(false);
            this->fcggroupBoxDeblock->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUDeblockThreshold))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUDeblockStrength))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUPsyRDOQ))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUPsyRD))->EndInit();
            this->fcggroupBoxFrame->ResumeLayout(false);
            this->fcggroupBoxFrame->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUHistThreshold))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURef))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUBframes))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUKeyintMax))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURCLookahead))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUScenecut))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUKeyintMin))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURD))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAQStrength))->EndInit();
            this->fcgGroupBoxME->ResumeLayout(false);
            this->fcgGroupBoxME->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURskipEdgeThreshold))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURskip))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUMaxMerge))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUMERange))->EndInit();
            this->fcgGroupBoxCTU->ResumeLayout(false);
            this->fcgGroupBoxCTU->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNULimitRefs))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUCtu))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUTuInterDepth))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUTuIntraDepth))->EndInit();
            this->fcgtabPageX2652pass->ResumeLayout(false);
            this->fcgtabPageX2652pass->PerformLayout();
            this->fcggroupBoxAnalysisReuse->ResumeLayout(false);
            this->fcggroupBoxAnalysisReuse->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURefineInter))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNURefineIntra))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAnalysisReuseLevel))->EndInit();
            this->fcgtabPageExSettings->ResumeLayout(false);
            this->fcgtabPageExSettings->PerformLayout();
            this->fchPNX265Sub->ResumeLayout(false);
            this->fchPNX265Sub->PerformLayout();
            this->fcggroupBoxCmdEx->ResumeLayout(false);
            this->fcggroupBoxCmdEx->PerformLayout();
            this->fcggroupBoxExSettings->ResumeLayout(false);
            this->fcggroupBoxExSettings->PerformLayout();
            this->fcgtoolStripSettings->ResumeLayout(false);
            this->fcgtoolStripSettings->PerformLayout();
            this->fcgtabControlMux->ResumeLayout(false);
            this->fcgtabPageMP4->ResumeLayout(false);
            this->fcgtabPageMP4->PerformLayout();
            this->fcgtabPageMKV->ResumeLayout(false);
            this->fcgtabPageMKV->PerformLayout();
            this->fcgtabPageMux->ResumeLayout(false);
            this->fcgtabPageMux->PerformLayout();
            this->fcgtabPageBat->ResumeLayout(false);
            this->fcgtabPageBat->PerformLayout();
            this->fcgCSExeFiles->ResumeLayout(false);
            this->fcgtabControlAudio->ResumeLayout(false);
            this->fcgtabPageAudioMain->ResumeLayout(false);
            this->fcgtabPageAudioMain->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fcgNUAudioBitrate))->EndInit();
            this->fcgtabPageAudioOther->ResumeLayout(false);
            this->fcgtabPageAudioOther->PerformLayout();
            this->fcgPNHideTabControlMux->ResumeLayout(false);
            this->fcgPNHideTabControlAudio->ResumeLayout(false);
            this->fcgPNHideTabControlVideo->ResumeLayout(false);
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
    private:
        TabPageCtrl^ fcgTabPageVideoController;
        CONF_ENC *cnf_fcgTemp;
        const SYSTEM_DATA *sys_dat;
        std::vector<std::string> *list_lng;
        CONF_GUIEX *conf;
        LocalSettings LocalStg;
        DarkenWindowStgReader *dwStgReader;
        AuoTheme themeMode;
        TBValueBitrateConvert TBBConvert;
        System::Threading::Timer^ qualityTimerX265;
        int timerChangeValue;
        bool CurrentPipeEnabled;
        bool stgChanged;
        String^ CurrentStgDir;
        ToolStripMenuItem^ CheckedStgMenuItem;
        CONF_GUIEX *cnf_stgSelected;
        String^ lastQualityStr;
    private:
        System::Void CheckTheme();
        System::Void SetAllMouseMove(Control ^top, const AuoTheme themeTo);
        System::Void fcgMouseEnter_SetColor(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgMouseLeave_SetColor(System::Object^  sender, System::EventArgs^  e);
        System::Void TabControl_DarkDrawItem(System::Object^ sender, DrawItemEventArgs^ e);

        System::Void LoadLangText();
        System::Int32 GetCurrentAudioDefaultBitrate();
        delegate System::Void qualityTimerChangeDelegate();
        System::Void qualityTimerChangeX265(Object^ state);
        System::Void fcgTBQualityChange();
        System::Void InitTimer();
        System::Void InitComboBox();
        System::Void setAudioDisplay();
        System::Void AudioEncodeModeChanged();
        System::Void InitStgFileList();
        System::Void RebuildStgFileDropDown(String^ stgDir);
        System::Void RebuildStgFileDropDown(ToolStripDropDownItem^ TS, String^ dir);
        System::Void SetLocalStg();
        System::Void LoadLocalStg();
        System::Void SaveLocalStg();
        System::Boolean CheckLocalStg();
        System::Void SetTXMaxLen(TextBox^ TX, int max_len);
        System::Void SetTXMaxLenAll();
        System::Void InitForm();
        System::Void ConfToFrm(CONF_GUIEX *cnf, bool all);
        System::Void FrmToConf(CONF_GUIEX *cnf);
        System::Void SetChangedEvent(Control^ control, System::EventHandler^ _event);
        System::Void SetToolStripEvents(ToolStrip^ TS, System::Windows::Forms::MouseEventHandler^ _event);
        System::Void SetAllCheckChangedEvents(Control ^top);
        System::Void SaveToStgFile(String^ stgName);
        System::Void DeleteStgFile(ToolStripMenuItem^ mItem);
        System::Boolean EnableSettingsNoteChange(bool Enable);
        System::Void fcgTSLSettingsNotes_DoubleClick(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgTSTSettingsNotes_Leave(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgTSTSettingsNotes_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
        System::Void fcgTSTSettingsNotes_TextChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void GetfcgTSLSettingsNotes(char *notes, int nSize);
        System::Void SetfcgTSLSettingsNotes(const char *notes);
        System::Void SetfcgTSLSettingsNotes(String^ notes);
        System::Void fcgTSBSave_Click(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgTSBSaveNew_Click(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgTSBDelete_Click(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgTSSettings_DropDownItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e);
        System::Void UncheckAllDropDownItem(ToolStripItem^ mItem);
        ToolStripMenuItem^ fcgTSSettingsSearchItem(String^ stgPath, ToolStripItem^ mItem);
        ToolStripMenuItem^ fcgTSSettingsSearchItem(String^ stgPath);
        System::Void CheckTSSettingsDropDownItem(ToolStripMenuItem^ mItem);
        System::Void CheckTSItemsEnabled(CONF_GUIEX *current_conf);

        System::Void InitLangList();
        System::Void SaveSelectedLanguage(const char *language_text);
        System::Void SetSelectedLanguage(const char *language_text);
        System::Void CheckTSLanguageDropDownItem(ToolStripMenuItem^ mItem);
        System::Void fcgTSLanguage_DropDownItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e);

        System::Void SetHelpToolTips();
        System::Void SetHelpToolTipsColorMatrixX265(Control^ control, const char *type);
        System::Void SetX265VersionToolTip(String^ x265Path);
        System::Void ShowExehelp(String^ ExePath, String^ args);
        System::Void fcgTSBOtherSettings_Click(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgChangeEnabled(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgChangeMuxerVisible(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgTSBBitrateCalc_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void CloseBitrateCalc();
        System::Void SetfbcBTABEnable(bool enable, int max);
        System::Void SetfbcBTVBEnable(bool enable);
        System::Void fcgCBUsehighbit_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgTSBRearrageTabs_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgCBAudio2pass_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgCBAFS_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgCXAudioEncoder_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgCXAudioEncMode_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgCXX265Mode_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgTXQuality_Enter(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgTXQuality_TextChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgTXQuality_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e);
        System::Void SetTBValueToTextBoxX265();
        System::Void AdjustLocation();
        System::Void ActivateToolTip(bool Enable);
        System::Void SetStgEscKey(bool Enable);
        System::Void fcgArrangeForAutoMultiPass(bool enable);
        System::Boolean fcgLastX265ModeAsAMP;
        System::Void AdjustCXDropDownWidth(ComboBox^ CX);
        System::Void InitCXCmdExInsert();
        System::Void fcgCXCmdExInsert_FontChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgCXCmdExInsert_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
        System::Void fcgCheckAMPAutoBitrateEvent(System::Object^  sender, System::EventArgs^  e);
        System::Void AddCheckAMPAutoBitrateEvent();
    public:
        System::Void InitData(CONF_GUIEX *set_config, const SYSTEM_DATA *system_data);
        System::Void SetVideoBitrate(int bitrate);
        System::Void SetAudioBitrate(int bitrate);
        System::Void InformfbcClosed();
    private:
        System::Void AddfcgLBAMPAutoBitrate() {
            //fcgLBAMPAutoBitrateには拡張した簡易透過ラベルを使用する(背景透過&マウスイベント透過)
            //普通に作成しておくと、フォームデザイナが使用できなくなり厄介なので、ここで作っておく
            fcgLBAMPAutoBitrate = gcnew TransparentLabel();
            fcgPNBitrate->Controls->Add(fcgLBAMPAutoBitrate);
            fcgLBAMPAutoBitrate->ForeColor = System::Drawing::Color::OrangeRed;
            fcgLBAMPAutoBitrate->Location = System::Drawing::Point(90, 30);
            fcgLBAMPAutoBitrate->Name = L"fcgLBAMPAutoBitrate";
            fcgLBAMPAutoBitrate->Size = System::Drawing::Size(181, 30);
            fcgLBAMPAutoBitrate->TabIndex = 15;
            fcgLBAMPAutoBitrate->Visible = false;
            fcgLBAMPAutoBitrate->Text = LOAD_CLI_STRING(AuofcgLBAMPAutoBitrate);
            fcgLBAMPAutoBitrate->BringToFront();
        }
    private:
        System::Void fcgTSItem_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
            EnableSettingsNoteChange(false);
        }
    private:
        System::Void frmConfig_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
            if (e->KeyCode == Keys::Escape)
                this->Close();
            if ((e->KeyData & (Keys::Control | Keys::Shift | Keys::Enter)) == (Keys::Control | Keys::Shift | Keys::Enter))
                fcgBTOK_Click(sender, nullptr);
        }
    private:
        System::Void NUSelectAll(System::Object^  sender, System::EventArgs^  e) {
            NumericUpDown^ NU = dynamic_cast<NumericUpDown^>(sender);
            NU->Select(0, NU->Text->Length);
        }
    private:
        System::Void setComboBox(ComboBox^ CX, const ENC_OPTION_STR * list) {
            CX->BeginUpdate();
            const int prevIdx = CX->SelectedIndex;
            CX->Items->Clear();
            for (int i = 0; list[i].desc; i++) {
                String^ string = nullptr;
                if (list[i].mes != AUO_MES_UNKNOWN) {
                    string = LOAD_CLI_STRING(list[i].mes);
                } 
                if (string == nullptr || string->Length == 0) {
                    string = String(list[i].desc).ToString();
                }
                CX->Items->Add(string);
            }
            SetCXIndex(CX, prevIdx);
            CX->EndUpdate();
        }
    private:
        System::Void setComboBox(ComboBox^ CX, const char * const * list) {
            CX->BeginUpdate();
            const int prevIdx = CX->SelectedIndex;
            CX->Items->Clear();
            for (int i = 0; list[i]; i++)
                CX->Items->Add(String(list[i]).ToString());
            SetCXIndex(CX, prevIdx);
            CX->EndUpdate();
        }
    private:
        System::Void setComboBox(ComboBox^ CX, const WCHAR * const * list) {
            CX->BeginUpdate();
            const int prevIdx = CX->SelectedIndex;
            CX->Items->Clear();
            for (int i = 0; list[i]; i++)
                CX->Items->Add(String(list[i]).ToString());
            SetCXIndex(CX, prevIdx);
            CX->EndUpdate();
        }
    private:
        System::Void setPriorityList(ComboBox^ CX) {
            CX->BeginUpdate();
            const int prevIdx = CX->SelectedIndex;
            CX->Items->Clear();
            for (int i = 0; priority_table[i].text; i++) {
                String^ string = nullptr;
                if (priority_table[i].mes != AUO_MES_UNKNOWN) {
                    string = LOAD_CLI_STRING(priority_table[i].mes);
                }
                if (string == nullptr || string->Length == 0) {
                    string = String(priority_table[i].text).ToString();
                }
                CX->Items->Add(string);
            }
            SetCXIndex(CX, prevIdx);
            CX->EndUpdate();
        }
    private:
        System::Void setMuxerCmdExNames(ComboBox^ CX, int muxer_index) {
            CX->BeginUpdate();
            const int prevIdx = CX->SelectedIndex;
            CX->Items->Clear();
            MUXER_SETTINGS *mstg = &sys_dat->exstg->s_mux[muxer_index];
            for (int i = 0; i < mstg->ex_count; i++)
                CX->Items->Add(String(mstg->ex_cmd[i].name).ToString());
            SetCXIndex(CX, prevIdx);
            CX->EndUpdate();
        }
    private:
        System::Void setAudioEncoderNames() {
            fcgCXAudioEncoder->BeginUpdate();
            const int prevIdx = fcgCXAudioEncoder->SelectedIndex;
            fcgCXAudioEncoder->Items->Clear();
            //fcgCXAudioEncoder->Items->AddRange(reinterpret_cast<array<String^>^>(LocalStg.audEncName->ToArray(String::typeid)));
            fcgCXAudioEncoder->Items->AddRange(LocalStg.audEncName->ToArray());
            SetCXIndex(fcgCXAudioEncoder, prevIdx);
            fcgCXAudioEncoder->EndUpdate();
        }
    private:
        System::Void TX_LimitbyBytes(System::Object^  sender, System::ComponentModel::CancelEventArgs^ e) {
            int maxLength = 0;
            int stringBytes = 0;
            TextBox^ TX = nullptr;
            if ((TX = dynamic_cast<TextBox^>(sender)) == nullptr)
                return;
            stringBytes = CountStringBytes(TX->Text);
            maxLength = TX->MaxLength;
            if (stringBytes > maxLength - 1) {
                e->Cancel = true;
                MessageBox::Show(this, LOAD_CLI_STRING(AUO_CONFIG_TEXT_LIMIT_LENGTH), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
    private:
        System::Boolean openExeFile(TextBox^ TX, String^ ExeName) {
            //WinXPにおいて、OpenFileDialogはCurrentDirctoryを勝手に変更しやがるので、
            //一度保存し、あとから再適用する
            String^ CurrentDir = Directory::GetCurrentDirectory();
            OpenFileDialog ofd;
            ofd.Multiselect = false;
            ofd.FileName = ExeName;
            ofd.Filter = MakeExeFilter(ExeName);
            if (Directory::Exists(LocalStg.LastAppDir))
                ofd.InitialDirectory = Path::GetFullPath(LocalStg.LastAppDir);
            else if (File::Exists(TX->Text))
                ofd.InitialDirectory = Path::GetFullPath(Path::GetDirectoryName(TX->Text));
            else
                ofd.InitialDirectory = String(sys_dat->aviutl_dir).ToString();
            bool ret = (ofd.ShowDialog() == System::Windows::Forms::DialogResult::OK);
            if (ret) {
                if (sys_dat->exstg->s_local.get_relative_path)
                    ofd.FileName = GetRelativePath(ofd.FileName, CurrentDir);
                LocalStg.LastAppDir = Path::GetDirectoryName(Path::GetFullPath(ofd.FileName));
                TX->Text = ofd.FileName;
                TX->SelectionStart = TX->Text->Length;
            }
            Directory::SetCurrentDirectory(CurrentDir);
            return ret;
        }
    private:
        System::Void fcgBTX265Path_Click(System::Object^  sender, System::EventArgs^  e) {
            if (openExeFile(fcgTXX265Path, LocalStg.x265ExeName))
                SetX265VersionToolTip(fcgTXX265Path->Text);
        }
    private:
        System::Void fcgBTX265PathSub_Click(System::Object^  sender, System::EventArgs^  e) {
            if (openExeFile(fcgTXX265PathSub, LocalStg.x265ExeName))
                SetX265VersionToolTip(fcgTXX265PathSub->Text);
        }
    private:
        System::Void frmConfig::ExeTXPathEnter() {
            fcgTXX265Path_Enter(nullptr, nullptr);
            fcgTXX265PathSub_Enter(nullptr, nullptr);
            fcgTXAudioEncoderPath_Enter(nullptr, nullptr);
            fcgTXMP4MuxerPath_Enter(nullptr, nullptr);
            fcgTXTC2MP4Path_Enter(nullptr, nullptr);
            fcgTXMP4RawPath_Enter(nullptr, nullptr);
            fcgTXMKVMuxerPath_Enter(nullptr, nullptr);
        }
    private:
        System::Void frmConfig::ExeTXPathLeave() {
            fcgTXX265Path_Leave(nullptr, nullptr);
            fcgTXX265PathSub_Leave(nullptr, nullptr);
            fcgTXAudioEncoderPath_Leave(nullptr, nullptr);
            fcgTXMP4MuxerPath_Leave(nullptr, nullptr);
            fcgTXTC2MP4Path_Leave(nullptr, nullptr);
            fcgTXMP4RawPath_Leave(nullptr, nullptr);
            fcgTXMKVMuxerPath_Leave(nullptr, nullptr);
        }
    private:
        System::Void fcgTXX265Path_Enter(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXX265Path->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                fcgTXX265Path->Text = L"";
            }
        }
    private:
        System::Void fcgTXX265Path_Leave(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXX265Path->Text->Length == 0) {
                fcgTXX265Path->Text = LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH);
            }
            SetX265VersionToolTip(fcgTXX265Path->Text);
        }
    private:
        System::Void fcgTXX265PathSub_Enter(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXX265PathSub->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                fcgTXX265PathSub->Text = L"";
            }
        }
    private:
        System::Void fcgTXX265PathSub_Leave(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXX265PathSub->Text->Length == 0) {
                fcgTXX265PathSub->Text = LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH);
            }
            SetX265VersionToolTip(fcgTXX265PathSub->Text);
        }
    private:
        System::Void fcgTXAudioEncoderPath_Enter(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXAudioEncoderPath->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                fcgTXAudioEncoderPath->Text = L"";
            }
        }
    private:
        System::Void fcgTXAudioEncoderPath_Leave(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXAudioEncoderPath->Text->Length == 0) {
                fcgTXAudioEncoderPath->Text = LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH);
            }
        }
    private:
        System::Void fcgTXMP4MuxerPath_Enter(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXMP4MuxerPath->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                fcgTXMP4MuxerPath->Text = L"";
            }
        }
    private:
        System::Void fcgTXMP4MuxerPath_Leave(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXMP4MuxerPath->Text->Length == 0) {
                fcgTXMP4MuxerPath->Text = LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH);
            }
        }
    private:
        System::Void fcgTXTC2MP4Path_Enter(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXTC2MP4Path->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                fcgTXTC2MP4Path->Text = L"";
            }
        }
    private:
        System::Void fcgTXTC2MP4Path_Leave(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXTC2MP4Path->Text->Length == 0) {
                fcgTXTC2MP4Path->Text = LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH);
            }
        }
    private:
        System::Void fcgTXMP4RawPath_Enter(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXMP4RawPath->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                fcgTXMP4RawPath->Text = L"";
            }
        }
    private:
        System::Void fcgTXMP4RawPath_Leave(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXMP4RawPath->Text->Length == 0) {
                fcgTXMP4RawPath->Text = LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH);
            }
        }
    private:
        System::Void fcgTXMKVMuxerPath_Enter(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXMKVMuxerPath->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                fcgTXMKVMuxerPath->Text = L"";
            }
        }
    private:
        System::Void fcgTXMKVMuxerPath_Leave(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXMKVMuxerPath->Text->Length == 0) {
                fcgTXMKVMuxerPath->Text = LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH);
            }
        }
    private:
        System::Void fcgBTMP4MuxerPath_Click(System::Object^  sender, System::EventArgs^  e) {
            openExeFile(fcgTXMP4MuxerPath, LocalStg.MP4MuxerExeName);
        }
    private:
        System::Void fcgBTTC2MP4Path_Click(System::Object^  sender, System::EventArgs^  e) {
            openExeFile(fcgTXTC2MP4Path, LocalStg.TC2MP4ExeName);
        }
    private:
        System::Void fcgBTMP4RawMuxerPath_Click(System::Object^  sender, System::EventArgs^  e) {
            openExeFile(fcgTXMP4RawPath, LocalStg.MP4RawExeName);
        }
    private:
        System::Void fcgBTAudioEncoderPath_Click(System::Object^  sender, System::EventArgs^  e) {
            int index = fcgCXAudioEncoder->SelectedIndex;
            openExeFile(fcgTXAudioEncoderPath, LocalStg.audEncExeName[index]);
        }
    private:
        System::Void fcgBTMKVMuxerPath_Click(System::Object^  sender, System::EventArgs^  e) {
            openExeFile(fcgTXMKVMuxerPath, LocalStg.MKVMuxerExeName);
        }
    private:
        System::Void openTempFolder(TextBox^ TX) {
            FolderBrowserDialog^ fbd = fcgfolderBrowserTemp;
            if (Directory::Exists(TX->Text))
                fbd->SelectedPath = TX->Text;
            if (fbd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                if (sys_dat->exstg->s_local.get_relative_path)
                    fbd->SelectedPath = GetRelativePath(fbd->SelectedPath);
                TX->Text = fbd->SelectedPath;
                TX->SelectionStart = TX->Text->Length;
            }
        }
    private:
        System::Void fcgBTCustomAudioTempDir_Click(System::Object^  sender, System::EventArgs^  e) {
            openTempFolder(fcgTXCustomAudioTempDir);
        }
    private:
        System::Void fcgBTMP4BoxTempDir_Click(System::Object^  sender, System::EventArgs^  e) {
            openTempFolder(fcgTXMP4BoxTempDir);
        }
    private:
        System::Void fcgBTCustomTempDir_Click(System::Object^  sender, System::EventArgs^  e) {
            openTempFolder(fcgTXCustomTempDir);
        }
    private:
        System::Boolean openAndSetFilePath(TextBox^ TX, String^ fileTypeName) {
            return openAndSetFilePath(TX, fileTypeName, nullptr, nullptr);
        }
    private:
        System::Boolean openAndSetFilePath(TextBox^ TX, String^ fileTypeName, String^ ext) {
            return openAndSetFilePath(TX, fileTypeName, ext, nullptr);
        }
    private:
        System::Boolean openAndSetFilePath(TextBox^ TX, String^ fileTypeName, String^ ext, String^ dir) {
            //WinXPにおいて、OpenFileDialogはCurrentDirctoryを勝手に変更しやがるので、
            //一度保存し、あとから再適用する
            String^ CurrentDir = Directory::GetCurrentDirectory();
            //設定
            if (ext == nullptr)
                ext = L".*";
            OpenFileDialog^ ofd = fcgOpenFileDialog;
            ofd->FileName = L"";
            if (dir != nullptr && Directory::Exists(dir))
                ofd->InitialDirectory = dir;
            if (TX->Text->Length) {
                String^ fileName = nullptr;
                try {
                    fileName = Path::GetFileName(TX->Text);
                } catch (...) {
                    //invalid charによる例外は破棄
                }
                if (fileName != nullptr)
                    ofd->FileName = fileName;
            }
            ofd->Multiselect = false;
            ofd->Filter = fileTypeName + L"(*" + ext + L")|*" + ext;
            bool ret = (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK);
            if (ret) {
                if (sys_dat->exstg->s_local.get_relative_path)
                    ofd->FileName = GetRelativePath(ofd->FileName, CurrentDir);
                TX->Text = ofd->FileName;
                TX->SelectionStart = TX->Text->Length;
            }
            Directory::SetCurrentDirectory(CurrentDir);
            return ret;
        }
    private:
        System::Void fcgBTStatusFile_Click(System::Object^  sender, System::EventArgs^  e) {
            openAndSetFilePath(fcgTXStatusFile, LOAD_CLI_STRING(AUO_CONFIG_STATUS_FILE));
        }
    private:
        System::Void fcgBTAnalysisReuseFile_Click(System::Object^  sender, System::EventArgs^  e) {
            openAndSetFilePath(fcgTXAnalysisReuseFile, LOAD_CLI_STRING(AUO_CONFIG_ANALYSIS_FILE));
        }
    private:
        System::Void fcgBTBatBeforePath_Click(System::Object^  sender, System::EventArgs^  e) {
            if (openAndSetFilePath(fcgTXBatBeforePath, LOAD_CLI_STRING(AUO_CONFIG_BAT_FILE), ".bat", LocalStg.LastBatDir))
                LocalStg.LastBatDir = Path::GetDirectoryName(fcgTXBatBeforePath->Text);
        }
    private:
        System::Void fcgBTBatAfterPath_Click(System::Object^  sender, System::EventArgs^  e) {
            if (openAndSetFilePath(fcgTXBatAfterPath, LOAD_CLI_STRING(AUO_CONFIG_BAT_FILE), ".bat", LocalStg.LastBatDir))
                LocalStg.LastBatDir = Path::GetDirectoryName(fcgTXBatAfterPath->Text);
        }
    private:
        System::Void fcgBTBatBeforeAudioPath_Click(System::Object^  sender, System::EventArgs^  e) {
            if (openAndSetFilePath(fcgTXBatBeforeAudioPath, LOAD_CLI_STRING(AUO_CONFIG_BAT_FILE), ".bat", LocalStg.LastBatDir))
                LocalStg.LastBatDir = Path::GetDirectoryName(fcgTXBatBeforeAudioPath->Text);
        }
    private:
        System::Void fcgBTBatAfterAudioPath_Click(System::Object^  sender, System::EventArgs^  e) {
            if (openAndSetFilePath(fcgTXBatAfterAudioPath, LOAD_CLI_STRING(AUO_CONFIG_BAT_FILE), ".bat", LocalStg.LastBatDir))
                LocalStg.LastBatDir = Path::GetDirectoryName(fcgTXBatAfterAudioPath->Text);
        }
    private:
        System::Void SetCXIndex(ComboBox^ CX, int index) {
            if (CX->Items->Count > 0) {
                CX->SelectedIndex = clamp(index, 0, CX->Items->Count - 1);
            }
        }
    private:
        System::Void SetNUValue(NumericUpDown^ NU, Decimal d) {
            NU->Value = clamp(d, NU->Minimum, NU->Maximum);
        }
    private:
        System::Void SetNUValue(NumericUpDown^ NU, int i) {
            NU->Value = clamp(Convert::ToDecimal(i), NU->Minimum, NU->Maximum);
        }
    private:
        System::Void SetNUValue(NumericUpDown^ NU, unsigned int i) {
            NU->Value = clamp(Convert::ToDecimal(i), NU->Minimum, NU->Maximum);
        }
    private:
        System::Void SetNUValue(NumericUpDown^ NU, float f) {
            NU->Value = clamp(Convert::ToDecimal(f), NU->Minimum, NU->Maximum);
        }
    private:
        System::Void SetNUValue(NumericUpDown^ NU, double d) {
            NU->Value = clamp(Convert::ToDecimal(d), NU->Minimum, NU->Maximum);
        }
    private:
        System::Int32 SetTBValue(TrackBar^ TB, int value) {
            int i = clamp(value, TB->Minimum, TB->Maximum);
            TB->Value = i;
            return i;
        }
    private:
        System::Void fcgTBQuality_Scroll(System::Object^  sender, System::EventArgs^  e) {
            SetTBValueToTextBoxX265();
        }
    private:
        System::Void fcgTBQuality_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
            if (fcgLBAMPAutoBitrate != nullptr)
                fcgLBAMPAutoBitrate->Invalidate(); //上に乗っかっているので再描画をかける
        }
    private:
        System::Void fcgBTTBQualitySubtract_Click(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTBQuality->Value > fcgTBQuality->Minimum) {
                fcgTBQuality->Value--;
                SetTBValueToTextBoxX265();
            }
        }
    private:
        System::Void fcgBTTBQualityAdd_Click(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTBQuality->Value < fcgTBQuality->Maximum) {
                fcgTBQuality->Value++;
                SetTBValueToTextBoxX265();
            }
        }
    private:
        System::Void fcgCBNulOut_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
            if (fcgCBNulOut->Enabled)
                cnf_fcgTemp->nul_out = fcgCBNulOut->Checked;
        }
    private:
        System::Void fcgCBFastFirstPass_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
            if (fcgCBFastFirstPass->Enabled)
                cnf_fcgTemp->slow_first_pass = !fcgCBFastFirstPass->Checked;
        }
    private:
        System::Void fcgBTApplyPreset_Click(System::Object^  sender, System::EventArgs^  e) {
            CONF_GUIEX cnf;
            init_CONF_GUIEX(&cnf, get_bit_depth(fcgCXBitDepth->SelectedIndex) > 8);
            cnf.enc.preset = fcgCXPreset->SelectedIndex;
            cnf.enc.tune = fcgCXTune->SelectedIndex;
            cnf.enc.profile = fcgCXProfile->SelectedIndex;
            apply_presets(&cnf.enc);
            ConfToFrm(&cnf, false);
        }
    private:
        System::Void fcgBTCmdEx_Click(System::Object^  sender, System::EventArgs^  e) {
            CONF_GUIEX cnf;
            init_CONF_GUIEX(&cnf, get_bit_depth(fcgCXBitDepth->SelectedIndex) > 8);
            FrmToConf(&cnf);
            char cmdex[2048] = { 0 };
            GetCHARfromString(cmdex, sizeof(cmdex), fcgTXCmdEx->Text);
            set_cmd_to_conf(cmdex, &cnf.enc);
            ConfToFrm(&cnf, false);
        }
    private:
        System::Void frmConfig_Load(System::Object^  sender, System::EventArgs^  e) {
            InitForm();
        }
    private:
        System::Void fcgBTOK_Click(System::Object^  sender, System::EventArgs^  e) {
            if (CheckLocalStg())
                return;
            init_CONF_GUIEX(conf, get_bit_depth(fcgCXBitDepth->SelectedIndex) > 8);
            FrmToConf(conf);
            SaveLocalStg();
            ZeroMemory(conf->oth.notes, sizeof(conf->oth.notes));
            this->Close();
        }
    private:
        System::Void fcgBTCancel_Click(System::Object^  sender, System::EventArgs^  e) {
            this->Close();
        }
    private:
        System::Void fcgBTDefault_Click(System::Object^  sender, System::EventArgs^  e) {
            CONF_GUIEX confDefault;
            init_CONF_GUIEX(&confDefault, FALSE);
            ConfToFrm(&confDefault, true);
        }
    private:
        System::Void ChangePresetNameDisplay(bool changed) {
            if (CheckedStgMenuItem != nullptr) {
                fcgTSSettings->Text = (changed) ? L"[" + CheckedStgMenuItem->Text + L"]*" : CheckedStgMenuItem->Text;
                fcgTSBSave->Enabled = changed;
            }
        }
    private:
        System::Void fcgRebuildCmd(System::Object^  sender, System::EventArgs^  e) {
            char re_cmd[MAX_CMD_LEN] = { 0 };
            CONF_GUIEX rebuild;
            init_CONF_GUIEX(&rebuild, get_bit_depth(fcgCXBitDepth->SelectedIndex) > 8);
            FrmToConf(&rebuild);
            if (!rebuild.oth.disable_guicmd) {
                build_cmd_from_conf(re_cmd, sizeof(re_cmd), &rebuild.enc, &rebuild.vid, FALSE);
            }
            fcgTXCmd->Text = String(re_cmd).ToString();
            if (CheckedStgMenuItem != nullptr)
                ChangePresetNameDisplay(memcmp(&rebuild, cnf_stgSelected, sizeof(CONF_GUIEX)) != 0);
        }
    private:
        System::Void CheckOtherChanges(System::Object^  sender, System::EventArgs^  e) {
            if (CheckedStgMenuItem == nullptr)
                return;
            CONF_GUIEX check_change;
            init_CONF_GUIEX(&check_change, get_bit_depth(fcgCXBitDepth->SelectedIndex) > 8);
            FrmToConf(&check_change);
            ChangePresetNameDisplay(memcmp(&check_change, cnf_stgSelected, sizeof(CONF_GUIEX)) != 0);
        }
    private:
        System::Void fcgTXCmd_DoubleClick(System::Object^  sender, System::EventArgs^  e) {
            int offset = (fcgTXCmd->Multiline) ? -fcgTXCmdfulloffset : fcgTXCmdfulloffset;
            fcgTXCmd->Height += offset;
            this->Height += offset;
            fcgTXCmd->Multiline = !fcgTXCmd->Multiline;
        }
    private:
        System::Void fcgTSSettings_Click(System::Object^  sender, System::EventArgs^  e) {
            if (EnableSettingsNoteChange(false))
                fcgTSSettings->ShowDropDown();
        }
    private:
        System::Void fcgBTTBQualityAdd_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
            timerChangeValue = 1;
            qualityTimerX265->Change(fcgTBQualityTimerLatency, fcgTBQualityTimerPeriod);
        }
    private:
        System::Void fcgBTTBQualityAdd_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
            qualityTimerX265->Change(System::Threading::Timeout::Infinite, fcgTBQualityTimerPeriod);
        }
    private:
        System::Void fcgBTTBQualitySubtract_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
            timerChangeValue = -1;
            qualityTimerX265->Change(fcgTBQualityTimerLatency, fcgTBQualityTimerPeriod);
        }
    private:
        System::Void fcgBTTBQualitySubtract_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
            qualityTimerX265->Change(System::Threading::Timeout::Infinite, fcgTBQualityTimerPeriod);
        }
    private:
        System::Void fcgTXX265Path_TextChanged(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXX265Path->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                fcgTXX265PathSub->Text = LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH);
                LocalStg.x265Path = L"";
                fcgTXX265Path->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Disabled);
            } else {
                int c = fcgTXX265Path->SelectionStart;
                LocalStg.x265Path = fcgTXX265Path->Text;
                fcgTXX265Path->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Normal);
                fcgTXX265PathSub->Text = LocalStg.x265Path;
                fcgTXX265PathSub->SelectionStart = fcgTXX265PathSub->Text->Length;
                fcgTXX265Path->SelectionStart = c;
                fcgBTX265Path->ContextMenuStrip = (File::Exists(fcgTXX265Path->Text)) ? fcgCSExeFiles : nullptr;
            }
        }
    private:
        System::Void fcgTXX265PathSub_TextChanged(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXX265PathSub->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                fcgTXX265Path->Text = LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH);
                LocalStg.x265Path = L"";
                fcgTXX265PathSub->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Disabled);
            } else {
                LocalStg.x265Path = fcgTXX265PathSub->Text;
                fcgTXX265PathSub->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Normal);
                int c = fcgTXX265PathSub->SelectionStart;
                fcgTXX265Path->Text = LocalStg.x265Path;
                fcgTXX265Path->SelectionStart = fcgTXX265Path->Text->Length;
                fcgTXX265PathSub->SelectionStart = c;
                fcgBTX265PathSub->ContextMenuStrip = (File::Exists(fcgTXX265PathSub->Text)) ? fcgCSExeFiles : nullptr;
            }
        }
    private:
        System::Void fcgTXX265PathSubhighbit_Leave(System::Object^  sender, System::EventArgs^  e) {
        }
    private:
        System::Void fcgTXAudioEncoderPath_TextChanged(System::Object^  sender, System::EventArgs^  e) {
            if (fcgCXAudioEncoder->SelectedIndex < 0) return;
            if (fcgTXAudioEncoderPath->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                LocalStg.audEncPath[fcgCXAudioEncoder->SelectedIndex] = L"";
                fcgTXAudioEncoderPath->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Disabled);
            } else {
                fcgTXAudioEncoderPath->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Normal);
                LocalStg.audEncPath[fcgCXAudioEncoder->SelectedIndex] = fcgTXAudioEncoderPath->Text;
                fcgBTAudioEncoderPath->ContextMenuStrip = (File::Exists(fcgTXAudioEncoderPath->Text)) ? fcgCSExeFiles : nullptr;
            }
        }
    private:
        System::Void fcgTXMP4MuxerPath_TextChanged(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXMP4MuxerPath->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                LocalStg.MP4MuxerPath = L"";
                fcgTXMP4MuxerPath->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Disabled);
            } else {
                fcgTXMP4MuxerPath->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Normal);
                LocalStg.MP4MuxerPath = fcgTXMP4MuxerPath->Text;
                fcgBTMP4MuxerPath->ContextMenuStrip = (File::Exists(fcgTXMP4MuxerPath->Text)) ? fcgCSExeFiles : nullptr;
            }
        }
    private:
        System::Void fcgTXTC2MP4Path_TextChanged(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXTC2MP4Path->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                LocalStg.TC2MP4Path = L"";
                fcgTXTC2MP4Path->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Disabled);
            } else {
                fcgTXTC2MP4Path->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Normal);
                LocalStg.TC2MP4Path = fcgTXTC2MP4Path->Text;
                fcgBTTC2MP4Path->ContextMenuStrip = (File::Exists(fcgTXTC2MP4Path->Text)) ? fcgCSExeFiles : nullptr;
            }
        }
    private:
        System::Void fcgTXMP4RawMuxerPath_TextChanged(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXMP4RawPath->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                LocalStg.MP4RawPath = L"";
                fcgTXMP4RawPath->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Disabled);
            } else {
                fcgTXMP4RawPath->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Normal);
                LocalStg.MP4RawPath = fcgTXMP4RawPath->Text;
                fcgBTMP4RawPath->ContextMenuStrip = (File::Exists(fcgTXMP4RawPath->Text)) ? fcgCSExeFiles : nullptr;
            }
        }
    private:
        System::Void fcgTXMKVMuxerPath_TextChanged(System::Object^  sender, System::EventArgs^  e) {
            if (fcgTXMKVMuxerPath->Text == LOAD_CLI_STRING(AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH)) {
                LocalStg.MKVMuxerPath = L"";
                fcgTXMKVMuxerPath->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Disabled);
            } else {
                fcgTXMKVMuxerPath->ForeColor = getTextBoxForeColor(themeMode, dwStgReader, DarkenWindowState::Normal);
                LocalStg.MKVMuxerPath = fcgTXMKVMuxerPath->Text;
                fcgBTMKVMuxerPath->ContextMenuStrip = (File::Exists(fcgTXMKVMuxerPath->Text)) ? fcgCSExeFiles : nullptr;
            }
        }
    private:
        System::Void fcgTXMP4BoxTempDir_TextChanged(System::Object^  sender, System::EventArgs^  e) {
            LocalStg.CustomMP4TmpDir = fcgTXMP4BoxTempDir->Text;
        }
    private:
        System::Void fcgTXCustomAudioTempDir_TextChanged(System::Object^  sender, System::EventArgs^  e) {
            LocalStg.CustomAudTmpDir = fcgTXCustomAudioTempDir->Text;
        }
    private:
        System::Void fcgTXCustomTempDir_TextChanged(System::Object^  sender, System::EventArgs^  e) {
            LocalStg.CustomTmpDir = fcgTXCustomTempDir->Text;
        }
    private:
        System::Void fcgSetDragDropFilename_Enter(System::Object^  sender, DragEventArgs^  e) {
            e->Effect = (e->Data->GetDataPresent(DataFormats::FileDrop)) ? DragDropEffects::Copy : DragDropEffects::None;
        }
    private:
        System::Void fcgSetDragDropFilename_DragDrop(System::Object^  sender, DragEventArgs^  e) {
            TextBox^ TX = dynamic_cast<TextBox^>(sender);
            array<System::String ^>^ filelist = dynamic_cast<array<System::String ^>^>(e->Data->GetData(DataFormats::FileDrop, false));
            if (filelist == nullptr || TX == nullptr)
                return;
            String^ filePath = filelist[0]; //複数だった場合は先頭のものを使用
            if (sys_dat->exstg->s_local.get_relative_path)
                filePath = GetRelativePath(filePath);
            TX->Text = filePath;
        }
    private:
        System::Void fcgInsertDragDropFilename_Enter(System::Object^  sender, DragEventArgs^  e) {
            e->Effect = (   e->Data->GetDataPresent(DataFormats::FileDrop)
                         || e->Data->GetDataPresent(DataFormats::Text)
                         || e->Data->GetDataPresent(DataFormats::UnicodeText)
                         || e->Data->GetDataPresent(DataFormats::Rtf)
                         || e->Data->GetDataPresent(DataFormats::Html)) ? DragDropEffects::Copy : DragDropEffects::None;
        }
    private:
        System::Void fcgInsertDragDropFilename_DragOver(System::Object^  sender, DragEventArgs^  e) {
            TextBox^ TX = dynamic_cast<TextBox^>(sender);
            if (TX == nullptr)
                return;
            TX->Focus();
            this->Activate();
            //文字列がない部分の座標を指定した時に文字列の長さを返さない(一つ前の文字)になるため厄介
            //int index = TX->GetCharIndexFromPosition(TX->PointToClient(Point(e->X, e->Y)));
            //そこでEM_CHARFROMPOSメッセージを送る方法を使用する
            Point pint = TX->PointToClient(Point(e->X, e->Y));
            int p = ((pint.X & 0xffff) | (pint.Y << 16));
            int index = ::SendMessage((HWND)TX->Handle.ToPointer(), EM_CHARFROMPOS, 0, p) & 0xffff;
            TX->SelectionStart = index;
            //行・桁位置を求めるには以下の方法があるが、まあ今回は使わない
            //int line = TX->GetLineFromCharIndex(index);
            //int column = index - TX->GetFirstCharIndexFromLine(line);
        }
    private:
        System::Void fcgInsertDragDropFilename_DragDrop(System::Object^  sender, DragEventArgs^  e) {
            TextBox^ TX = dynamic_cast<TextBox^>(sender);
            if (TX == nullptr)
                return;
            String^ insertStr;
            if (e->Data->GetDataPresent(DataFormats::FileDrop)) {
                array<System::String ^>^ filelist = dynamic_cast<array<System::String ^>^>(e->Data->GetData(DataFormats::FileDrop, false));
                if (filelist == nullptr)
                    return;
                for (int i = 0; i < filelist->Length; i++) {
                    if (i) insertStr += L" ";
                    insertStr += L"\"" + ((sys_dat->exstg->s_local.get_relative_path) ? GetRelativePath(filelist[i]) : filelist[i]) + L"\"";
                }
            } else {
                insertStr = dynamic_cast<System::String^>(e->Data->GetData(DataFormats::Text));
                if (insertStr == nullptr)
                    return;
            }
            int current_selection = TX->SelectionStart;
            TX->Text = TX->Text->Insert(TX->SelectionStart, insertStr);
            TX->SelectionStart = current_selection + insertStr->Length; //たまに変なところへ行くので念のため必要
        }
    private:
        System::Void fcgTSExeFileshelp_Click(System::Object^  sender, System::EventArgs^  e) {
            System::Windows::Forms::ToolStripMenuItem^ TS = dynamic_cast<System::Windows::Forms::ToolStripMenuItem^>(sender);
            if (TS == nullptr) return;
            System::Windows::Forms::ContextMenuStrip^ CS = dynamic_cast<System::Windows::Forms::ContextMenuStrip^>(TS->Owner);
            if (CS == nullptr) return;

            //Name, args, Path の順番
            array<ExeControls>^ ControlList = {
                { fcgBTX265Path->Name,           fcgTXX265Path->Text,           sys_dat->exstg->s_enc.help_cmd },
                { fcgBTX265PathSub->Name,        fcgTXX265PathSub->Text,        sys_dat->exstg->s_enc.help_cmd },
                { fcgBTAudioEncoderPath->Name,   fcgTXAudioEncoderPath->Text,   sys_dat->exstg->s_aud[fcgCXAudioEncoder->SelectedIndex].cmd_help },
                { fcgBTMP4MuxerPath->Name,       fcgTXMP4MuxerPath->Text,       sys_dat->exstg->s_mux[MUXER_MP4].help_cmd },
                { fcgBTTC2MP4Path->Name,         fcgTXTC2MP4Path->Text,         sys_dat->exstg->s_mux[MUXER_TC2MP4].help_cmd },
                { fcgBTMP4RawPath->Name,         fcgTXMP4RawPath->Text,         sys_dat->exstg->s_mux[MUXER_MP4_RAW].help_cmd },
                { fcgBTMKVMuxerPath->Name,       fcgTXMKVMuxerPath->Text,       sys_dat->exstg->s_mux[MUXER_MKV].help_cmd },
            };
            for (int i = 0; i < ControlList->Length; i++) {
                if (NULL == String::Compare(CS->SourceControl->Name, ControlList[i].Name)) {
                    ShowExehelp(ControlList[i].Path, String(ControlList[i].args).ToString());
                    return;
                }
            }
            MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_HELP_CMD_UNSET), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    private:
        System::Void fcgLBguiExBlog_LinkClicked(System::Object^  sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^  e) {
            fcgLBguiExBlog->LinkVisited = true;
            try {
                System::Diagnostics::Process::Start(String(sys_dat->exstg->blog_url).ToString());
            } catch (...) {
                //まあ放置
            };
        }
    private:
        System::Void fcgNUCtu_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
            NumericUpDown^ senderNU = dynamic_cast<NumericUpDown^>(sender);
            if (senderNU != nullptr) {
                int current_value = (int)senderNU->Value;
                int max_ctu_size = (int)senderNU->Maximum;
                int increment = (int)senderNU->Increment;

                int value_to_set = 0;
                for (int i = 0, pow2i = 1; 0==value_to_set && pow2i <= max_ctu_size; i++) {
                    if (current_value == pow2i)             value_to_set = pow2i;
                    if (current_value == pow2i - increment) value_to_set = std::max(pow2i / 2, (int)senderNU->Minimum);
                    if (current_value == pow2i + increment) value_to_set = std::min(pow2i * 2, (int)senderNU->Maximum);
                    pow2i *= 2;
                }
                if (0 == value_to_set) {
                    for (int i = 0, pow2i = 1; 0==value_to_set && pow2i <= max_ctu_size; i++) {
                        if (current_value < pow2i) {
                            value_to_set = clamp(pow2i, (int)senderNU->Minimum, (int)senderNU->Maximum);
                            break;
                        }
                        pow2i *= 2;
                    }
                }
                SetNUValue(senderNU, value_to_set);
            }
        }
    private:
        System::Void fcgCXSceneCutMode_SelectedIndexChanged(System::Object ^sender, System::EventArgs ^e) {
            const bool hist_scenecut_enabled = fcgCXSceneCutMode->SelectedIndex > 0;
            fcgNUHistThreshold->Visible = hist_scenecut_enabled;
            fcgNUScenecut->Visible = !hist_scenecut_enabled;
        }
};
}
