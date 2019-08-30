#pragma once

#include <vector>
#include <cstdlib>
#include <cassert>

// These includes are only for the debug print capability used during testing
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <iomanip>

template<typename TKey, typename TValue>
class SkipList
{
public:
    SkipList()
        : m_size(0),
        m_head(nullptr)
    {
    }

    TValue* find(const TKey& key)
    {
        if (m_head == nullptr) return nullptr;

        auto* node = findNode(key);
        if (node->key == key)
            return &node->value;

        return nullptr;
    }

    void insert(const TKey& key, TValue value)
    {
        // Don't have anything in the list yet
        if (m_head == nullptr)
        {
            m_head = new Node();
            m_head->key = key;
            m_head->value = std::move(value);
            m_head->nextNodes.push_back(nullptr);
            m_head->leftNode = nullptr;
            m_size++;
        }

        auto* node = findNode(key);
        if (node->key == key)
        {
            node->value = std::move(value);
        }
        else if (node->key > key) // node == m_head, new key must become new head
        {
            assert(node == m_head);

            auto* newNode = new Node();
            newNode->key = key;
            newNode->value = std::move(value);
            m_size++;

            auto* oldHead = m_head;
            swapIntoHead(newNode);
            bubbleUp(oldHead);
        }
        else // inserting into list
        {
            auto* newNode = new Node();
            newNode->key = key;
            newNode->value = std::move(value);
            newNode->leftNode = node;
            m_size++;

            // Insert into lowest level linked list
            newNode->nextNodes.push_back(node->nextNodes[0]);
            if (node->nextNodes[0] != nullptr)
                node->nextNodes[0]->leftNode = newNode;
            node->nextNodes[0] = newNode;

            bubbleUp(newNode);
        }
    }

    bool erase(const TKey& key)
    {
        // TODO: delete the node
        if (m_head == nullptr)
            return false;

        auto* node = findNode(key);
        if (node->key != key)
            return false;

        // If deleting the head, promote the next node to the head
        if (node == m_head)
        {
            // Head is the only node
            if (m_head->nextNodes[0] == nullptr)
            {
                delete m_head;
                m_head = nullptr;
                return true;
            }

            // TODO: can we do this swap without requiring
            // a copy of the nextNodes vector?
            auto nextNodes = m_head->nextNodes[0]->nextNodes;
            auto* oldHead = m_head;
            swapIntoHead(m_head->nextNodes[0]);
            // Now that the head has been swapped, carry on with deleting the old head node
            node = oldHead;
            node->nextNodes = nextNodes;
        }

        // Connect right nodes previous node to our left node
        if (node->nextNodes[0] != nullptr)
            node->nextNodes[0]->leftNode = node->leftNode;

        auto* leftNode = node->leftNode;
        for (size_t level = 0; level < node->nextNodes.size();)
        {
            // If the left node has something at this level,
            // connect it to this nodes right element
            if (leftNode->nextNodes.size() > level)
            {
                auto* rightNode = node->nextNodes[level];
                leftNode->nextNodes[level] = rightNode;
                // TODO: if rightNode is nullptr, does this allow us
                // to remove a level from the SkipList in some cases?

                level++; // Process the next level on the next iteration
            }
            // Left node does not have something at this level but
            // something further to the left may
            else
            {
                leftNode = leftNode->leftNode;
                level--; // Still need to process this level
            }
        }

        delete node;
        return true;
    }

    size_t size() const { return m_size; }

    void debugPrint()
    {
        std::stringstream ss;
        std::unordered_map<void*, size_t> indexMap;

        Node* node = m_head;
        size_t i = 0;
        while (node != nullptr)
        {
            indexMap[(void*)node] = i++;
            node = node->nextNodes[0];
        }

        for (size_t level = 0; level < m_head->nextNodes.size(); level++)
        {
            node = m_head;
            ss << std::to_string(m_head->key);
            i = 1;

            node = m_head->nextNodes[level];

            while (node != nullptr)
            {
                if (indexMap[(void*)node] == i)
                {
                    ss << std::setw(3) << std::to_string(node->key);
                    node = node->nextNodes[level];
                }
                else
                    ss << "   ";

                i++;
            }
            ss << std::endl;
        }

        std::cout << ss.str();
    }

private:
    // a node within the skip list
    struct Node
    {
        // The key value for this node
        TKey key;

        // The value stored at this node
        TValue value;

        // Pointers to the next nodes over from this node
        // from lowest level to highest
        std::vector<Node*> nextNodes;

        // The node left of this one at the lowest level on the list
        Node* leftNode;
    };

    // Inserts the node into higher levels with a 50% chance at each level
    void bubbleUp(Node* node)
    {
        size_t level = 1;

        // Flip a coin and half the time insert into a higher level
        auto* leftNode = node->leftNode;
        while (((double)rand() / RAND_MAX) > 0.5 && level < m_head->nextNodes.size())
        {
            // Find the closest node to the left at this level.
            // Note that there will always be a node as the head
            // is always at the highest possible level.
            while (leftNode->nextNodes.size() <= level)
                leftNode = leftNode->leftNode;

            // Insert into this level
            node->nextNodes.push_back(leftNode->nextNodes[level]);
            leftNode->nextNodes[level] = node;

            level++;
        }

        // This node has reached the heads level - increase heads level
        if (node->nextNodes.size() == m_head->nextNodes.size())
        {
            m_head->nextNodes.push_back(nullptr);
        }
    }

    void swapIntoHead(Node* node)
    {
        // Swap the new node and head
        node->nextNodes = m_head->nextNodes;
        node->leftNode = nullptr;
        m_head->leftNode = node;
        m_head->nextNodes.clear();
        m_head->nextNodes.push_back(node->nextNodes[0]);
        node->nextNodes[0] = m_head;

        m_head = node;
    }

    Node* findNode(const TKey& key)
    {
        assert(m_head != nullptr);

        auto* current = m_head;
        size_t level = current->nextNodes.size();
        for (; level-- > 0;)
        {
            while (current->nextNodes[level] != nullptr && current->nextNodes[level]->key <= key)
            {
                current = current->nextNodes[level];
            }
        }

        // At this point, current is either:
        // 1. the node containing the element
        // 2. the node just to the left of where the key should be on the
        //    lowest level of the skip list.
        // 3. the key is to the left of every element in the list and thus
        //    current == m_head
        return current;
    }

    size_t m_size;
    Node* m_head;
};
