#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type &val, Node *next)
            : value(val), next_node(next)
        {
        }
        Type value;
        Node *next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node) : node_(node) {}

    public:
        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_) {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
           return !(node_ == rhs.node_);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(node_ == rhs.node_);
        }

        // Оператор прединкремента.
        BasicIterator& operator++() noexcept {
            assert(node_);
            node_ = node_->next_node;
            
            return *this;
        }

        // Оператор постинкремента.
        BasicIterator operator++(int) noexcept {
            auto it(*this);
            ++*this;
            return it;

            return *this;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_);
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;

    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList() {}

    SingleLinkedList(std::initializer_list<Type> values) {
        Create(values.begin(), values.end());
    }

    SingleLinkedList(const SingleLinkedList& other) {
        Create(other.begin(), other.end());
    }

    ~SingleLinkedList() {
        Clear();
    }

    // Вставляет элемент спереди
    void PushFront(const Type &value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    // Очищает список за время O(N)
    void Clear() noexcept
    {
        while (head_.next_node != nullptr)
        {
            Node *node_to_delete = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete node_to_delete;
        }
        size_ = 0;
    }

    // Возвращает количество элементов в списке
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_);
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;

        return Iterator(new_node);
    }

    void PopFront() noexcept {
        assert(size_ > 0);
        Node* to_delete = head_.next_node;
        head_.next_node = to_delete->next_node;
        delete to_delete;
        --size_;
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ && pos.node_->next_node && size_ > 0);
        Node* node_to_delete = pos.node_->next_node;
        pos.node_->next_node = node_to_delete->next_node;
        delete node_to_delete;
        --size_;

        return Iterator(pos.node_->next_node);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this == rhs) {
            return this;
        }

        SingleLinkedList rhs_copy(rhs);
        swap(rhs_copy);

        return *this;
    }

    // Он почему то ругается на std (no matching function for call to 'swap')
    // Если переименовать функцию то всё ок
    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
    }

    //Итераторная область

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(head_);
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_ = 0;

    template <typename It>
    void Create(It begin, It end) {
        SingleLinkedList temp_list;
        Node* node = &temp_list.head_;
        size_t size_count = 0;
        while (begin != end) {
            Node* temp_node = new Node(*begin, nullptr);
            node->next_node = temp_node;
            node = node->next_node;
            ++begin;
            ++size_count;
        }

        node->next_node = nullptr;
        temp_list.size_ = size_count;
        swap(temp_list);
    }
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return  (&lhs == &rhs) || (lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs > rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
} 