
# 拡張 x265 出力(GUI) Ex  3.xx
by rigaya

拡張 x265 出力(GUI) Ex (x265guiEx) は、x265を使用してエンコードを行うAviutlの出力プラグインです。

## ダウンロード & 更新履歴
[rigayaの日記兼メモ帳＞＞](http://rigaya34589.blog135.fc2.com/blog-category-5.html)

## 基本動作環境
Windows 10 (x86/x64)  
Aviutl 1.00 以降  

WindowsXPでは動作しません。

## x265guiEx 使用にあたっての注意事項
無保証です。自己責任で使用してください。  
拡張 x265 出力(GUI) Exを使用したことによる、いかなる損害・トラブルについても責任を負いません。  

## iniファイルによる拡張
x265guiEx.iniを書き換えることにより、音声エンコーダやmuxerのコマンドラインを変更できます。また音声エンコーダを追加することもできます。

デフォルトの設定では不十分だと思った場合は、iniファイルの音声やmuxerのコマンドラインを調整してみてください。


## エンコ前後バッチ処理
出力したファイルに対してなんらかの処理をするためのものです。
指定されたbatファイルから特定の文字列を検索し、置換を行ったバッチファイルを実行します。

```
 -- 共通置換文字列 --
;コマンドライン部分で共通で使用できる置換名
%{vidpath} … 一時動画ファイル名(フルパス)
%{audpath} … 一時音声ファイル名(フルパス)
%{tmpdir}  … 一時フォルダ名(最後の\無し)
%{tmpfile} … 一時ファイル名(フルパス・拡張子除く)
%{tmpname} … 一時ファイル名(ファイル名のみ・拡張子除く)
%{savpath} … 出力ファイル名(フルパス)
%{savfile} … 出力ファイル名(フルパス・拡張子除く)
%{savname} … 出力ファイル名(ファイル名のみ・拡張子除く)
%{savdir}  … 出力フォルダ名(最後の\無し)
%{aviutldir} … Aviutl.exeのフォルダ名(最後の\無し)
%{chpath} … チャプターファイル名(フルパス)   (%{savfile}[chapter_appendix])
%{tcpath} … タイムコードファイル名(フルパス) (%{tmpfile}[tc_appendix])
%{muxout} … muxで作成する一時ファイル名(フルパス)
%{x265path}     … 指定された x265.exe のパス
%{x265_10path}  … 指定された x265.exe(10bit版) のパス
%{audencpath}   … 実行された音声エンコーダのパス
%{mp4muxerpath} … mp4 muxerのパス
%{mkvmuxerpath} … mkv muxerのパス
%{fps_scale}        … フレームレート(分母)
%{fps_rate}         … フレームレート(分子)
%{fps_rate_times_4} … フレームレート(分子)×4
%{sar_x} / %{par_x} … サンプルアスペクト比 (横)
%{sar_y} / %{par_y} … サンプルアスペクト比 (縦)
%{dar_x}            … 画面アスペクト比 (横)
%{dar_y}            … 画面アスペクト比 (縦)
 -- 以下エンコ後バッチファイルでのみ使用できるもの --
%{logpath}      … ログファイルのパス
 -- 以下はmuxを行った場合のみ --
%{chapter}      … チャプターファイルへのパス (チャプターを追加するよう指定した時のみ)
%{chap_apple}   … Apple形式のチャプターファイルへのパス (チャプターを追加するよう指定した時のみ)
```

## 注意事項
- 自動フィールドシフト使用上の制限  
自動フィールドシフト使用時はx265のvbv-bufsize,vbv-maxrateは正しく反映されません。
これは、エンコード中にタイムコードを取得して、mux時にタイムコードを反映させるためです。

- 2passの制限  
自動フィールドシフト(afs)を有効にした場合、
「自動マルチパス」以外の方法での2-pass(およびn-pass)エンコードは失敗します。
これは afsによるdrop数が2pass目以降
事前にわかっていなければならないためです。
自動フィールドシフトを使わない場合は問題ないです。


## ソースコードについて
- MITライセンスです。

### ソースの構成
VCビルド  
文字コード: UTF-8-BOM  
改行: CRLF  
インデント: 空白x4  
