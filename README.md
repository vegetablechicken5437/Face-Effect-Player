# Face Effect Player 人臉特效播放器

這是一個基於 OpenCV 的人臉偵測與特效處理專案。它能即時偵測視訊中的人臉，並提供多種有趣的特效選項，包含史瑞克綠臉、瘦臉、馬賽克以及雷射眼等。

-----

## 專案功能

  * **即時人臉偵測**：使用 Haar Cascade 分類器快速偵測視訊中的人臉。
  * **多樣化特效**：
      * **綠臉**：將人臉膚色轉換為類似史瑞克的綠色。
      * **瘦臉**：透過 remap 函式創造下巴變尖的瘦臉效果。
      * **馬賽克**：對人臉區域進行馬賽克處理，保護隱私或增加趣味性。
      * **雷射眼**：滑鼠點擊畫面後，從雙眼發射雷射光束，並在點擊位置產生爆破特效。
  * **使用者互動**：提供滑鼠點擊互動，讓使用者選擇不同的特效模式。

-----

## 如何開始

### 環境要求

  * **OpenCV**：這個專案完全依賴 OpenCV 函式庫。
  * **C++編譯器**：支援 C++11 或更高版本的編譯器（如 g++）。

### 安裝步驟

1.  **複製專案**：

    ```bash
    git clone https://github.com/vegetablechicken5437/face-effect-player.git
    cd face-effect-player
    ```

2.  **準備分類器檔案**：
    確保 `data` 資料夾中包含以下 Haar Cascade 分類器檔案：

      * `haarcascade_frontalface_alt.xml` (正面人臉偵測)
      * `haarcascade_eye_tree_eyeglasses.xml` (眼鏡人眼偵測)

3.  **準備視訊檔案**：

      * 確保 `data` 資料夾中包含 `explosion.mp4` 爆破視訊檔案。

4.  **編譯程式碼**：
    使用 g++ 和 `pkg-config` 進行編譯。

    ```bash
    g++ -o face_effect_player main.cpp `pkg-config --cflags --libs opencv4`
    ```

      * **注意**：`main.cpp` 替換為你的主程式檔名，`opencv4` 替換為你安裝的 OpenCV 版本。

### 使用方式

1.  **執行程式**：

    ```bash
    ./face_effect_player
    ```

2.  **操作說明**：

      * 程式執行後會自動開啟你的視訊鏡頭。
      * 在畫面下方有三個特效選項：`Green`、`Slim`、`Mosaic`。
      * 用滑鼠點擊選項文字即可切換特效。
      * 點擊畫面其他區域則會啟動**雷射眼**特效。
      * 按下鍵盤 `Esc` 鍵即可退出程式。

-----

## 專案截圖

https://github.com/user-attachments/assets/18aa9a61-7461-4bc8-a3ca-61a0eecaed25

-----

## 貢獻指南

歡迎所有形式的貢獻！如果你對這個專案有任何想法或想改進的地方，請隨時提出 Pull Request。

-----

## 授權

這個專案遵循 **MIT 授權**。
