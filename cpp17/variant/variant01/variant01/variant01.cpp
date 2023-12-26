#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include <variant>

enum class CharactorId
{
    Player,       // �v���C���[
    FriendAlice,  // ����
    FriendBob,
    EnemyEve,     // �ȉ��͓G
    /* ... */
};

// �Z�̎��
enum class OrderType
{
    Attack,    // �U��
    Buff,      // �o�t
    Summon,    // ����
    /* ... */
};

// ---------------------------------------------------
// ���ۂɂ��̃^�[���łǂ�ȋZ���o���̂��̏��(��0')
// �U��
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
    int hitPoint_;                 // �U����
};
// �o�t
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
    int buffPoint_;                 // �o�t��
};
// ����
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
    CharactorId summonFriendId_;            // �ꏏ�ɏ������閡��ID
};

// ---------------------------------------------------
// �L�����N�^�[ID���擾���邽�߂̍\����
struct GetCharactorId
{
    void operator()(const std::shared_ptr<OrderAttack>& order) const { order->GetCharactor(); }
    void operator()(const std::shared_ptr<OrderBuff>& order) const { order->GetCharactor(); }
    void operator()(const std::shared_ptr<OrderSummon>& order) const { order->GetCharactor(); }
};

// �Z�̎��s���܂Ƃ߂Ă���\����
struct ExecuteOrder
{
    void operator()(const std::shared_ptr<OrderAttack>& order) const { order->ExecuteOrder(); }
    void operator()(const std::shared_ptr<OrderBuff>& order) const { order->ExecuteOrder(); }
    void operator()(const std::shared_ptr<OrderSummon>& order) const { order->ExecuteOrder(); }
};

// ---------------------------------------------------
// �I�[�g�v���C�̎��܂Ƃ߃N���X
class AutoPlay
{
public:
    std::vector<std::variant<std::shared_ptr<OrderAttack>, std::shared_ptr<OrderBuff>, std::shared_ptr<OrderSummon>>> OrderList;

    // ���̃^�[���Ŏ��s�������A�S���̃I�[�g�v���C�̋Z�����s����
    void ExecuteAllOrder()
    {
        for (auto const& order : OrderList)
        {
            std::visit(ExecuteOrder{}, order);
        }
    }

    // ����̉����̒l���X�V������
    // ��j�v���C���[�̍U���͂����Ƃ���X�V
    void UpdatePlayerAttackPoint(int newHitPoint)
    {
        for (auto& order : OrderList)
        {
            // OrderAttack�I�u�W�F�N�g��T��
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

    // ���̋Z���o����̂��́A���o���Ă���Z�������肵�ēo�^���Ă���(��3')
    autoPlay->OrderList.emplace_back(std::make_shared<OrderAttack>(CharactorId::Player, 10));
    autoPlay->OrderList.emplace_back(std::make_shared<OrderSummon>(CharactorId::FriendAlice, CharactorId::FriendBob));

    // �Ⴆ�΁A�����̎��_�ŉ����o�t�����������Ƃ��āA����Player�ɓo�^����Ă���l�����̎��_�ōX�V�������A�Ƃ�����
    autoPlay->UpdatePlayerAttackPoint(20);

    autoPlay->OrderList.emplace_back(std::make_shared<OrderBuff>(CharactorId::FriendBob, 5));

    // ���s�������Z���������̂ŁA���s�I
    autoPlay->ExecuteAllOrder();
}
