#include <functional>
#include <iostream>
#include <vector>
#include <memory>

enum class CharactorId
{
    Player,       // プレイヤー
    FriendAlice,  // 味方
    FriendBob,
    EnemyEve,     // 以下は敵
    /* ... */
};

// 技の種類
enum class OrderType
{
    Attack,    // 攻撃
    Buff,      // バフ
    Summon,    // 召喚
    /* ... */
};

// 実際にそのターンでどんな技を出すのかの情報(★0)
class AutoPlayOrder
{
public:
    AutoPlayOrder(CharactorId id, OrderType type)
    {
        charactorId_ = id;
        CurrentOrderType = type;

        // enumでのタイプ分け(★1)
        switch (CurrentOrderType)
        {
        case OrderType::Attack:
            hitPoint_ = 10;     // default
            ExecuteFunction = [this]() {ExecuteAttack(); };
            break;
        case OrderType::Buff:
            buffPoint_ = 5;     // default
            ExecuteFunction = [this]() {ExecuteBuff(); };
            break;
        case OrderType::Summon:
        default:
            summonFriendId_ = CharactorId::FriendBob;   // default
            ExecuteFunction = [this]() {ExecuteSummon(); };
            break;
        }
    }

    OrderType CurrentOrderType;
    std::function<void()> ExecuteFunction;
    void ExecuteAttack() const { std::cout << "Attack:" << hitPoint_ << "\n"; }
    void ExecuteBuff() const { std::cout << "Buff:" << buffPoint_ << "\n"; }
    void ExecuteSummon() const { std::cout << "Summon:" << (int)summonFriendId_ << "\n"; }

private:
    CharactorId charactorId_;

    // 技によって必要なメンバ変数が異なる(★2)
    int hitPoint_;                 // orderType == Attack なら、攻撃力
    int buffPoint_;                // orderType == Buff なら、バフ量
    CharactorId summonFriendId_;   // orderType == Summon なら、一緒に召喚する味方ID
    /* ... */

};

// オートプレイの取りまとめクラス
class AutoPlay
{
public:
    std::vector<std::shared_ptr<AutoPlayOrder>> OrderList;
};

int main()
{
    std::cout << "===AutoPlay===\n";

    auto autoPlay = std::make_unique<AutoPlay>();

    // 今回は、とあるターンでの技が３つあったとする
    // 何の技が出せるのかは、今出している技を見たりするので結構複雑な処理になる(★3)
    autoPlay->OrderList.emplace_back(std::make_shared<AutoPlayOrder>(CharactorId::Player, OrderType::Attack));
    autoPlay->OrderList.emplace_back(std::make_shared<AutoPlayOrder>(CharactorId::FriendAlice, OrderType::Summon));
    autoPlay->OrderList.emplace_back(std::make_shared<AutoPlayOrder>(CharactorId::FriendBob, OrderType::Buff));

    // execute
    for (auto const& order : autoPlay->OrderList)
    {
        // オートプレイ実行時はポリモーフィックに出来てる(★4)
        order->ExecuteFunction();
    }
}
