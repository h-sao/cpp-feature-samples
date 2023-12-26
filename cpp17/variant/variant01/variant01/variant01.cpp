#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include <variant>

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

// ---------------------------------------------------
// 実際にそのターンでどんな技を出すのかの情報(★0')
// 攻撃
class OrderAttack
{
public:
    OrderAttack(CharactorId id, int hitPoint) :
        CurrentOrderType(OrderType::Attack),
        charactorId_(id),
        hitPoint_(hitPoint) {}

    OrderType CurrentOrderType;
    CharactorId GetCharactor() { return charactorId_; }
    void ExecuteOrder() { std::cout << "Attack:" << hitPoint_ << "\n"; }
    void UpdatePlayerAttack(int newHitPoint){hitPoint_ = newHitPoint;}


private:
    CharactorId charactorId_;
    int hitPoint_;                 // 攻撃力
};
// バフ
class OrderBuff
{
public:
    OrderBuff(CharactorId id, int buff) :
        CurrentOrderType(OrderType::Buff),
        charactorId_(id),
        buffPoint_(buff) {}

    OrderType CurrentOrderType;
    CharactorId GetCharactor() { return charactorId_; }
    void ExecuteOrder() { std::cout << "Buff:" << buffPoint_ << "\n"; }

private:
    CharactorId charactorId_;
    int buffPoint_;                 // バフ量
};
// 召喚
class OrderSummon
{
public:
    OrderSummon(CharactorId id, CharactorId summonFriend) :
        CurrentOrderType(OrderType::Summon),
        charactorId_(id),
        summonFriendId_(summonFriend) {}

    OrderType CurrentOrderType;
    CharactorId GetCharactor() { return charactorId_; }
    void ExecuteOrder() { std::cout << "Summon:" << (int)summonFriendId_ << "\n"; }

private:
    CharactorId charactorId_;
    CharactorId summonFriendId_;            // 一緒に召喚する味方ID
};

// ---------------------------------------------------
// キャラクターIDを取得するための構造体
struct GetCharactorId
{
    void operator()(const std::shared_ptr<OrderAttack>& order) const { order->GetCharactor(); }
    void operator()(const std::shared_ptr<OrderBuff>& order) const { order->GetCharactor(); }
    void operator()(const std::shared_ptr<OrderSummon>& order) const { order->GetCharactor(); }
};

// 技の実行をまとめている構造体
struct ExecuteOrder
{
    void operator()(const std::shared_ptr<OrderAttack>& order) const { order->ExecuteOrder(); }
    void operator()(const std::shared_ptr<OrderBuff>& order) const { order->ExecuteOrder(); }
    void operator()(const std::shared_ptr<OrderSummon>& order) const { order->ExecuteOrder(); }
};

// ---------------------------------------------------
// オートプレイの取りまとめクラス
class AutoPlay
{
public:
    std::vector<std::variant<std::shared_ptr<OrderAttack>, std::shared_ptr<OrderBuff>, std::shared_ptr<OrderSummon>>> OrderList;

    // このターンで実行したい、全部のオートプレイの技を実行する
    void ExecuteAllOrder()
    {
        for (auto const& order : OrderList)
        {
            std::visit(ExecuteOrder{}, order);
        }
    }

    // 特定の何かの値を更新したい
    // 例）プレイヤーの攻撃力をあとから更新
    void UpdatePlayerAttackPoint(int newHitPoint)
    {
        for (auto& order : OrderList)
        {
            // OrderAttackオブジェクトを探す
            auto attackOrder = std::get_if<std::shared_ptr<OrderAttack>>(&order);
            if (attackOrder && (*attackOrder)->GetCharactor() == CharactorId::Player)
            {
                (*attackOrder)->UpdatePlayerAttack(newHitPoint);
                break;
            }
        }
    }
};

int main()
{
    std::cout << "===AutoPlay===\n";

    auto autoPlay = std::make_unique<AutoPlay>();

    // 何の技が出せるのかは、今出している技を見たりして登録していく(★3')
    autoPlay->OrderList.emplace_back(std::make_shared<OrderAttack>(CharactorId::Player, 10));
    autoPlay->OrderList.emplace_back(std::make_shared<OrderSummon>(CharactorId::FriendAlice, CharactorId::FriendBob));

    // 例えば、ここの時点で何かバフがかかったとして、既にPlayerに登録されている値をこの時点で更新したい、という時
    autoPlay->UpdatePlayerAttackPoint(20);

    autoPlay->OrderList.emplace_back(std::make_shared<OrderBuff>(CharactorId::FriendBob, 5));

    // 実行したい技が整ったので、実行！
    autoPlay->ExecuteAllOrder();
}
