# robosys2020_work1

### 概要
ロボットシステム学の課題として作成．  
自然数を入力するとLEDの光を介してモールス信号に変換する．  
偶数は青LED，奇数は緑LEDが点灯し，信号の終わりには青LEDで０（モールス信号で）を出力する．

### 動作環境
・Rasberry Pi Model 3  
・Ubuntu 20.04

### 実行
`cd robosys2020_work1`  
`make`  
`sudo insmod myled.ko`  
`sudo chmod 666 /dev/myled0`  
`echo 変換したい自然数 > /dev/myled0`

### 配線例
・GPIO24番，GPIO25番，Grandを使用  
・LED2個  
・抵抗2個  
・ブレッドボード  



### デモ動画
1，0，5を順に入力している動画

### 参考
モールス信号変換・モールス信号解読｜Morse Decoder
<https://morsedecoder.com/ja/>
