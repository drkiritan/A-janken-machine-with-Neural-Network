#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
using namespace std;

// グー   -> 0
// チョキ -> 1
// パー   -> 2

// 過去N回のプレイヤーの手を記憶する
#define N 5

// 30回先に勝った方の勝利
#define LOOP_END 30

// デバッグモード
// 初期状態 -> false
bool debug = false;

// 表示する文字列
// グーチョキパーの数字に対応する
string sign[3] = {"Gu", "Choki", "Pa"};

class NeuralNetwork
{
public:
    // グーの予想素子   -> y[0]
    // チョキの予想素子 -> y[1]
    // パーの予想素子   -> y[2]
    int y[3] = {0};
    
    // 各予想素子への入力信号
    int v[3] = {0};
    
    // 教師信号
    // 前回のプレイヤーが出した手をもとに作成
    // グーならば   -> {1, -1, -1}
    // チョキならば -> {-1, 1, -1}
    // パーならば   -> {-1, -1, 1}
    int z[3] = {0};

    // バイアス
    // 発火のしやすさを調整
    int h[3] = {0};

    // 過去N回のプレイヤーが出した手を格納する
    // グーならば   -> {1, -1, -1}
    // チョキならば -> {-1, 1, -1}
    // パーならば   -> {-1, -1, 1}
    // を格納する
    int x[N][3] = {0};

    // 重み
    // プレイヤーがグーを選ぶと仮定したとき   -> w[0]
    // プレイヤーがチョキを選ぶと仮定したとき -> w[1]
    // プレイヤーがパーを選ぶと仮定したとき   -> w[2]
    // を使用する
    // [N][3]は過去N回目におけるグーチョキパーそれぞれの重み
    int w[3][N][3] = {0};

    // 前回のプレイヤーが出した手を格納する
    // 初回はプレイヤーの過去データがないので、乱数で初期化
    int last_player_hand = rand() % 3;

    // 全ての処理を総括したメソッド
    int thinking();

    // 必要なデータの作成
    void inputData();

    // 誤り訂正
    void errorCorrection();

    // データの更新
    void upDate();

    // WTA則にしたがって、次のプレイヤーの手を予測
    int WinnerTakesAll();

    // v -> y に変換する
    int sgn(int);

    // デバッグモード時に全ての情報が表示される
    void showThinking();
};

int NeuralNetwork::thinking()
{
    // 思考の全体
    inputData();
    errorCorrection();
    upDate();
    return WinnerTakesAll();
}

void NeuralNetwork::inputData()
{
    // 前回のプレイヤーが出した手をもとに教師データを作成
    for (int i = 0; i < 3; i++)
        z[i] = -1;
    z[last_player_hand] = 1;

    // 前回の各予想素子の入力信号vを各予想素子yに変換
    for (int i = 0; i < 3; i++)
        y[i] = sgn(v[i]);
}

void NeuralNetwork::errorCorrection()
{
    // z と y の符号が一致
    // -> 予測した結果と実際の結果が一致
    // -> 誤り訂正を行わない
    //
    // z と y の符号が異なる
    // -> 予測した結果と実際の結果が異なる
    // -> 誤り訂正を実行
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < N; j++)
            for (int k = 0; k < 3; k++)
                w[i][j][k] += x[j][k] * (z[i] - y[i]) / 2;
        h[i] -= (z[i] - y[i]) / 2;
    }
}

void NeuralNetwork::upDate()
{
    // x の先頭に新しい過去データ z を格納
    for (int i = 0; i < N - 1; i++)
        for (int j = 0; j < 3; j++)
            x[N - i - 1][j] = x[N - i - 2][j];
    for (int i = 0; i < 3; i++)
        x[0][i] = z[i];
}

int NeuralNetwork::WinnerTakesAll()
{
    // v の初期化
    for (int i = 0; i < 3; i++)
        v[i] = 0;

    // プレイヤーの出す可能性が高い手 -> v[i] が大きい
    // プレイヤーの出す可能性が低い手 -> v[i] が小さい
    // 確率を算出しているわけではないことに注意
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < 3; k++)
                v[i] += w[i][j][k] * x[j][k] - h[i];

    // v[i] が一番大きいときの i（予想された手）を返却
    int max = -10000;
    int predict_hand = 0;
    for (int i = 0; i < 3; i++)
        if (v[i] > max)
        {
            max = v[i];
            predict_hand = i;
        }
    return predict_hand;
}

int NeuralNetwork::sgn(int vk)
{
    // v は様々な値を取るので、-1 もしくは 1 に変換する
    if (0 <= vk) return 1;
    return -1;
}

void NeuralNetwork::showThinking()
{
    // 現在の各変数や配列の状態を表示
    cout << "z" << endl;
    for (int i = 0; i < 3; i++)
        cout << z[i] << ", ";
    cout << endl << endl;

    cout << "x" << endl;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < 3; j++)
            cout << x[i][j] << ", ";
        cout << endl;
    }
    cout << endl;

    for (int i = 0; i < 3; i++)
    {
        cout << "w[" << i << "]:" << endl;
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < 3; k++)
                cout << w[i][j][k] << ", ";
            cout << endl;
        }
        cout << endl;
    }

    cout << "v" << endl;
    for (int i = 0; i < 3; i++)
        cout << v[i] << ", ";
    cout << endl << endl;
}

// プレイヤー、コンピュータの親クラス
class Character
{
public:
    Character(string);

    int wins;
    int hand;
    string name;

    void showHand();
};

Character::Character(string name)
{
    this->name = name;
}

void Character::showHand()
{
    cout << name << " : " << sign[hand] << endl;
}

class Computer : public Character
{
public:
    using Character::Character;
    void setHand();
    NeuralNetwork* brain = new NeuralNetwork;
};

void Computer::setHand()
{
    int predict_hand = brain->thinking();

    // 予想した手に勝つような手をセット
    // 予想した手がグー   : 0 -> (0 + 2) % 3 = 2 -> パーをセット
    // 予想した手がチョキ : 1 -> (1 + 2) % 3 = 0 -> グーをセット
    // 予想した手がパー   : 2 -> (2 + 2) % 3 = 1 -> チョキをセット
    hand = (predict_hand + 2) % 3;
    if (debug)
    {
        brain->showThinking();
        cout << "The machine's next hand -> " << sign[hand] << endl;
    }
}

class Player : public Character
{
public:
    using Character::Character;
    void setHand();
    int inputHand(string);
};

void Player::setHand()
{
    hand = inputHand("Input : ");
    while (hand < 1 || 3 < hand)
        hand = inputHand("Retry : ");
    hand--;
}

int Player::inputHand(string msg)
{
    // 数字以外の文字を入力した際のエラー回避
    string s;
    cout << msg;
    cin >> s;
    return atoi(s.c_str());
}

class GameControl
{
public:
    GameControl();
    ~GameControl();

    Player* p = new Player("YOU");
    Computer* c = new Computer("COM");

    int draw = 0;

    void game();
    void showTitle();
    void showMsg();
    void remainingProcess();
};

GameControl::GameControl()
{
    srand((unsigned int)time(NULL));
    showTitle();
    game();
}

GameControl::~GameControl()
{
    if (p->wins == LOOP_END) cout << "You won the game!" << endl;
    if (c->wins == LOOP_END) cout << "Computer won the game!" << endl;
    delete p;
    delete c;
}

void GameControl::game()
{
    while (p->wins < LOOP_END && c->wins < LOOP_END)
    {
        c->setHand();
        p->setHand();
        cout << endl;
        showMsg();
        cout << endl;
        c->showHand();
        p->showHand();
        cout << endl;
        remainingProcess();
        cout << endl;
    }
}

void GameControl::showTitle()
{
    cout << "---------------------------------------" << endl;
    cout << "----  Janken Game   -------------------" << endl;
    cout << "---------------------------------------" << endl;
    cout << "---  vs. janken machine   -------------" << endl;
    cout << "--------------  with Neural Network ---" << endl;
    cout << "---------------------------------------" << endl;
    cout << endl;
    cout << "1. Press number key to select your hand." << endl;
    cout << "2. Press Enter key to submit it." << endl;
    cout << endl;
    cout << "First one to win 30 times is the winner." << endl;
    cout << "[ GU ... 1 / CH ... 2 / PA ... 3 ]" << endl;
    cout << endl;
}

void GameControl::showMsg()
{
    cout << "++++++++++++++++" << endl;
    cout << "+++++ Pon! +++++" << endl;
    cout << "++++++++++++++++" << endl;
}

void GameControl::remainingProcess()
{
    if (p->hand == (c->hand + 2) % 3)
    {
        cout << p->name << " won!" << endl;
        p->wins++;
    }
    if (c->hand == (p->hand + 2) % 3)
    {
        cout << c->name << " won!" << endl;
        c->wins++;
    }
    if (p->hand == c->hand)
    {
        cout << "draw!" << endl;
        draw++;
    }
    cout << endl;
    cout << "Score" << endl;
    cout << p->name << " : " << p->wins << " // " << c->name << " : " << c->wins << " // draw : " << draw << endl;
    c->brain->last_player_hand = p->hand;
}

int main(int argc, char* argv[])
{
    // 実行時、何かしらのコマンドライン引数を検知するとデバッグモードがオンになる
    if (1 < argc) debug = true;
    GameControl* GameObj = new GameControl;
    delete GameObj;
    return 0;
}