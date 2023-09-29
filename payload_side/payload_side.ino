// マスター側のAruduinoプログラム
// PixhawkからPWM信号を受信し、投下装置の制御を行う
#include <Servo.h> 
#include <Stepper.h>


// 投下装置に合わせて調整してください。
const int SERVO_OPEN_POSITION = 180; // 投下時のサーボ位置(ロック解除)
const int SERVO_CLOSE_POSITION = 0; // 待機時のサーボ位置

// ボビンの巻き数（ここで指定した分だけ糸を繰り出す）
// 正転：+、反転：-
int revolution = 30;

// CUBEの設定に合わせて設定
const int DROP_THRESHOLD = 1300; // PWM信号の閾値、これ以上の値なら投下


Servo sv;

const int PWM_PIN = 2; // PixhawkからPWM信号を受信するピン
const int UNLOCK_SERVO_PIN = 9; // ロックを解除するサーボに接続するピン


// モーターの1回転ステップ数(オリエンタルモーターPKP246D15A-L)
int around_step = 200;

int step = around_step * revolution;

// ステッピングモータ回転速度(rpm)
int speed = 200;


// stepper インスタンス生成
Stepper stepper(around_step, 4,5,6,7);

void setup() {
  pinMode(PWM_PIN, INPUT);
  sv.attach(UNLOCK_SERVO_PIN);
  attachInterrupt(digitalPinToInterrupt(PWM_PIN), pwmReceived, CHANGE); // PWM信号の変化を割り込みとして扱う
  // 回転スピード設定
  stepper.setSpeed(speed) ; 
  Serial.begin(9600);
}

void loop() {
  // メインループでの処理が必要な場合はここに追加
}

void pwmReceived() {
  int pwmValue = pulseIn(PWM_PIN, HIGH); // PWM信号のパルス幅をマイクロ秒単位で取得
  Serial.println(pwmValue);
  if (pwmValue < DROP_THRESHOLD) {
      holdPayload(); // 投下物を保持
    }
    
  if (pwmValue >= DROP_THRESHOLD) {
    releasePayload(); // 投下シーケンスを実行
  }
}

void holdPayload() {
  Serial.println("Holding payload.");
  sv.write(SERVO_CLOSE_POSITION); // サーボをロックポジション
}

void releasePayload() {
  // 投下シーケンスを実装

  // 1. 投下信号をスレーブへ送信
  int dropSignal = 30; // 3桁の整数で表される投下信号
  Serial.println(dropSignal); // 信号をシリアル通信で送信
  Serial.println("Drop signal sended. Unlocking...");

  // 2. ロック解除
  sv.write(SERVO_OPEN_POSITION);
  delay(500); // ロック解除のための適切な時間を設定

  // 3. ステッピングモータを回してワイヤーを繰り出す（必要に応じて実装）
  stepper.step(step);

  // 4. ステッピングモータを回してワイヤーを巻き取る（必要に応じて実装）
  stepper.step(-1 * step);

  // 投下が完了したら、必要な後処理をここに追加
}
