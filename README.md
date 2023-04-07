# spi-uart
センサアクセスプログラム（関数）

dps310(気圧センサ)：spi接続
minigps(みちびき対応GPS):uart接続//DMAで10Hzで受信し、char型データをint,double型に変換しタイマー割り込みで更新
mpu9250(9軸センサ):spi接続//ジャイロ、加速度を1000Hzで受信し、地磁気センサを内部スレーブ機能からI２Cを介して100Hzで受信する。
タイマー割り込みで更新し、madgwickフィルタ関数に代入し、クオータニオン値として姿勢を自動更新。
twelite(通信機):uart接続
HC-SR04(超音波センサ):degitalread(HIorLO)
