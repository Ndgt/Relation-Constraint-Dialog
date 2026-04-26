[README - English](/README.md) | [DeepWiki](https://deepwiki.com/Ndgt/Relation-Constraint-Dialog)

# Relation Constraint Dialog
MotionBuilder の Relation Constraint において手軽にオブジェクト配置ができる、検索ダイアログを提供するプラグインです。

<p align = "center">
<img src="docs/images-readme/demo.gif"><br>
</p>


<br>
<br>

## 環境
- **MotionBuilder: 2022 ~ 2027**

    **注意**: v3.0 から MotionBuilder 2020 はサポート対象外となりました。

- **OS: Windows**

<br>
<br>

## 想定される用途と制限事項
> [!CAUTION]
> このプラグインは、MotionBuilder SDK が正式にサポートしていない内部仕様に基づいて実装されています。MotionBuilder のUIが Qt をベースに作られているらしいことを踏まえ、Qt のライブラリを活用できる範囲で開発していますが、内部仕様は将来的に予告なく変更される可能性があります。ご利用は自己責任でお願いいたします。

このプラグインは、以下のような**本番環境での使用は推奨されません**：

- ライブ演出やリアルタイム操作

代わりに、以下の用途に限定してご利用ください：

- オフライン編集
- 事前セットアップ作業

念のため、**本番環境ではインストールしたプラグインを `plugins`フォルダから除去してください**。




<br>
<br>

## インストール

1. [Releases](https://github.com/Ndgt/Relation-Constraint-Dialog/releases) ページから最新版をダウンロード

2. ダウンロードした zip ファイルを展開

3. 使用している MotionBuilder のバージョンを確認<br>

   （例：MotionBuilder 2027 を使用している場合は `...-MB2027.dll` ）

4. 対応する `.dll` ファイルと `RelationConstraintDialogConfig.ini` を既定のプラグインフォルダ（`MotionBuilder <version>/bin/x64/plugins`）にコピー

    （**注意**: `plugins` フォルダへのコピーの際、管理者権限が必要になる場合があります。）

<br>
<br>

## ドキュメント
詳細なドキュメントは DeepWiki ページをご覧ください: [DeepWiki](https://deepwiki.com/Ndgt/Relation-Constraint-Dialog)

[Doxygen](https://www.doxygen.nl/) をインストールしている場合、ローカルでドキュメントをビルドできます。`doxygen <path/to/docs/Doxygen/Doxyfile>` を実行し、生成された `html` フォルダ内の `index.html` を参照してください。

<br>
<br>

## 使い方
- **Tab キー** - 検索ダイアログの表示 / 検索オプションの切り替え

    （**注意**: **Relation View 上にマウスカーソルがある**場合のみ表示されます）

    <p align = "center">
    <img src="docs/images-readme/usage_show.gif" width=80%><br>
    </p>

- **上下 キー** - 候補リストの移動

    <p align = "center">
    <img src="docs/images-readme/usage_select.gif" width=80%><br>
    </p>

- **Enter キー / クリック** - Operator やモデルの選択の確定、オブジェクト作成

    <p align = "center">
    <img src="docs/images-readme/usage_finalize.gif" width=80%><br>
    </p>

- **Esc キー / ダイアログ外部クリック** - オブジェクト作成をせずダイアログを終了

<br>

> [!TIP]
> もし Relation View 上でオブジェクトが期待した位置（マウスカーソルの位置）に作成されない場合は、**Relation View 上にマウスカーソルがある状態で A キーを押してください**。<br>
> このプラグインは Relation View 上のマウス操作を監視し、オブジェクトを作成する位置を計算します。そのため、View が「フリーズ」していた場合（マウスでドラッグしたがビューが動かなかった場合）、誤った位置に計算されることがあります。

<br>
<br>

## 開発
### 開発環境
1. **Visual Studio Build Tools および "C++ によるデスクトップ開発" ワークロード**

    - MSVC v143 - VS 2022 C++ x64/x86 ビルドツール : MotionBuilder 2024 ~
    - MSVC v142 - VS 2019 C++ x64/x86 ビルドツール : MotionBuilder 2022, 2023

<br>

2. **Qt - qtbase(必須)**, **qttools**(任意)

    このプラグインは **qtbase**（特に QtCore, QtGui, QtWidgets モジュール） を使用して開発されています。また、[Qt Widgets Designer](https://doc.qt.io/qt-6/qtdesigner-manual.html) によるUI作成のため **qttools** モジュールを利用しています。
    - Qt 6.8.3: MotionBuilder 2027
    - Qt 6.5.3: MotionBuilder 2025, 2026
    - Qt 5.15.2 : MotionBuilder 2022 ~ 2024


    まず [Qt Group Web サイト](https://www.qt.io/) で Qt Account を作成し、Qt Maintenance Tool をダウンロードしてください。その後、Maintenance Tool を使用して必要な Qt コンポーネント（Open Source 版）をインストールします。

<br>

3. **vcpkg**

    このプロジェクトでは、プロジェクト設定と依存関係管理に vcpkg を使用しています。以下のコマンドで vcpkg をセットアップしてください。

    ```
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg; .\bootstrap-vcpkg.bat
    ```

<br>

### 推奨ビルド環境

バージョン番号をクリックして、各公式ドキュメントを適宜参照してください。

- Qt ソース

    | Qt バージョン  | Visual Studio バージョン |
    |---------------|-------------------------|
    | Qt [6.8.3](https://doc.qt.io/qt-6.8/supported-platforms.html) | Visual Studio 2022 |
    | Qt [6.5.3](https://doc.qt.io/qt-6.5/supported-platforms.html) | Visual Studio 2019 or 2022| 
    |  Qt [5.15.2](https://doc-snapshots.qt.io/qt5-5.15/supported-platforms.html) | Visual Studio 2019 |

<br>

- MotionBuilder プラグイン

    | MotionBuilder バージョン | Visual Studio バージョン |
    |-------------------------|-------------------------|
    | [2024](https://help.autodesk.com/view/MOBPRO/2024/ENU/?guid=GUID-0C6F6BA3-E9A2-40D0-87AE-F4F8CF20A66C), [2025](https://help.autodesk.com/view/MOBPRO/2025/ENU/?guid=GUID-0C6F6BA3-E9A2-40D0-87AE-F4F8CF20A66C), [2026](https://help.autodesk.com/view/MOBPRO/2026/ENU/?guid=GUID-0C6F6BA3-E9A2-40D0-87AE-F4F8CF20A66C), [2027](https://help.autodesk.com/view/MOBPRO/2027/ENU/?guid=GUID-0C6F6BA3-E9A2-40D0-87AE-F4F8CF20A66C) | Visual Studio 2022 |
    | [2022](https://help.autodesk.com/view/MOBPRO/2022/ENU/?guid=GUID-0C6F6BA3-E9A2-40D0-87AE-F4F8CF20A66C), [2023](https://help.autodesk.com/view/MOBPRO/2023/ENU/?guid=GUID-0C6F6BA3-E9A2-40D0-87AE-F4F8CF20A66C)| Visual Studio 2019 |

<br>


### UIのカスタマイズ
検索ダイアログと設定ダイアログの UI は、Qt Widgets Designer を使用してカスタマイズできます。編集する際は `.ui` ファイルを Designer で開いてください。   

<p align = "center">
<img src="docs/images-readme/qt_widgets_designer.png"><br>
</p>
<br>

UI の編集を終えたら、**uic** (User Interface Compiler) を用いて `.ui` ファイルをヘッダファイル `.h` に変換します。

```
path/to/Qt/bin/uic.exe SearchDialog.ui -o ui_SearchDialog.h
```

> [!Note]
> `AUTOUIC` CMake プロパティにより、ビルド時に `.ui`ファイルのヘッダファイルへの変換が自動で行われるので、この処理は必要ではありません。開発時に統合開発環境における自動補完等の機能を利用されたい場合は実行してください。

<br>

### プラグインのビルド
1. **管理者権限で** ターミナルを起動

<br>

2. このリポジトリをクローン

    ```
    git clone https://github.com/Ndgt/Relation-Constraint-Dialog.git
    cd Relation-Constraint-Dialog/src
    ```

<br>

3.  [CMakePresets.json](./CMakePresets.json) のユーザー固有の変数の値を編集

    環境に合わせて、例えば以下の変数の値を設定してください。
    - `generator`: 使用する CMake generator
    - `MOBU_ROOT`: MotionBuilder のインストールパス
    - `QT_SOURCE_SEARCH_PATH`: Qt ソースのインストールパス

    <br>

    ```json
    {
      "version": 6,
      "configurePresets": [
        {
          "name": "base",
          "hidden": true,
          "architecture": {
            "strategy": "set",
            "value": "x64"
          },
          "binaryDir": "${sourceDir}/build/${presetName}",
          "generator": "Visual Studio 18 2026",
          "toolset": {
              "strategy": "set"
          },
          "cacheVariables": {
              "CMAKE_AUTOMOC": "ON",
              "CMAKE_AUTOUIC": "ON",
              "CMAKE_CXX_STANDARD": "17",
              "CMAKE_CXX_STANDARD_REQUIRED": "ON",

          ...

      {
        "name": "MotionBuilder2027",
        "inherits": [
          "base"
        ],
        "toolset": {
          "value": "v143"
        },
        "cacheVariables": {
          "MOBU_ROOT": "C:/Program Files/Autodesk/MotionBuilder 2027",
          "PRODUCT_VERSION": "2027",
          "QT_SOURCE_SEARCH_PATH": "C:/Qt/6.8.3/msvc2022_64"
        }
      },
    ```

<br>

4. プラグインのビルドおよびインストール

    ```
    cmake --preset MotionBuilder2027 --fresh
    cmake --build --preset 2027-RelWithDebInfo --clean-first
    cmake --install build/MotionBuilder2027 --config RelWithDebInfo
    ```


    ビルドされたプラグイン `.dll` ファイルと `RelationConstraintDialogConfig.ini` ファイルが既定のフォルダ `MotionBuilder <version>/bin/x64/plugins` にコピーされます。

    <br>**注意**: Visual Studio Code を使用している場合は、Tasks を使用してこれらのコマンドを実行できます。[tasks.json](./.vscode/tasks.json) を参照してください。


<br>
<br>

## 依存関係

- このプロジェクトは Qt フレームワーク（Community Edition）をダイナミックリンクで使用します。

    プロジェクトにおいて使用している Qt のコンポーネントは、GNU Lesser General Public License バージョン3（LGPLv3）のもとでライセンスされています。

    ライセンスドキュメントは、このリポジトリに含めています（[LICENSES/Qt/lgpl-3.0.txt](/LICENSES/Qt/lgpl-3.0.txt)）。また、GNU の[ウェブサイト](https://www.gnu.org/licenses/lgpl-3.0.en.html)でライセンスの原文を閲覧することもできます。

    Qt のライセンスに関する詳細は、[Qt Companyのライセンスページ](https://www.qt.io/ja-jp/qt-licensing) をご覧ください。

<br>

- このプロジェクトはまた、[Microsoft Detours](https://github.com/microsoft/Detours) を使用しています。

    Detours は MIT License のもとでライセンスされています。MIT ライセンスの条文は、このリポジトリに含めています（[LICENSES/Detours/LICENSE.txt](/LICENSES/Detours/LICENSE.txt)）。

<br>

## ライセンス

このプロジェクトは BSD 3-Clause License のもとでライセンスされています。詳細は [LICENSE](/LICENSE) ファイルをご覧ください。

このプロジェクト自体はBSD 3-Clause License のもとでライセンスされていますが、Qt の使用は LGPLv3 に、Detours の使用は MIT ライセンスに従います。バイナリの再配布は、これらのライセンスの条項を遵守する必要があります。