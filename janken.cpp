#include <iostream>
#include <string>

#define N 5
#define LOOP_END 30

using namespace std;

string sign[3] = {"Gu", "Choki", "Pa"};
bool debug = false;

class NeuralNetwork
{
public:
    int v[3] = {0};
    int z[3] = {0};
    int x[N][3] = {0};
    int w[3][N][3] = {0};
    int last_player_hand = 0;

    int thinking();
    void inputData();
    void errorCorrection();
    void upDate();
    void WinnerTakesAll();
    int getPredict();

    void showTinking();
};

int NeuralNetwork::thinking()
{
    inputData();
    errorCorrection();
    upDate();
    WinnerTakesAll();
    return getPredict();
}

void NeuralNetwork::inputData()
{
    for (int i = 0; i < 3; i++)
        z[i] = -1;
    z[last_player_hand] = 1;
}

void NeuralNetwork::errorCorrection()
{
    for (int i = 0; i < 3; i++)
        if (z[i] * v[i] <= 0)
            for (int j = 0; j < N; j++)
                for (int k = 0; k < 3; k++)
                    w[i][j][k] += z[i] * x[j][k];
}

void NeuralNetwork::upDate()
{
    for (int i = 0; i < N - 1; i++)
        for (int j = 0; j < 3; j++)
            x[N - i - 1][j] = x[N - i - 2][j];
    for (int i = 0; i < 3; i++)
        x[0][i] = z[i];
}

void NeuralNetwork::WinnerTakesAll()
{
    for (int i = 0; i < 3; i++)
        v[i] = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < 3; k++)
                v[i] += w[i][j][k] * x[j][k];
}

int NeuralNetwork::getPredict()
{
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

void NeuralNetwork::showTinking()
{
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
    hand = (predict_hand + 2) % 3;
    if (debug)
    {
        brain->showTinking();
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
    cout << "---------------------" << endl;
    cout << "-----Janken Game-----" << endl;
    cout << "---------------------" << endl;
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
    if (1 < argc) debug = true;
    GameControl* GameObj = new GameControl;
    delete GameObj;
    return 0;
}