#include <vector>
#include <list>
#include <stdexcept>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {

private:
    std::vector<std::vector<std::pair<const KeyType, ValueType>>>   htable;
    Hash                                                            hasher_;
    size_t                                                          size_;


public:
    HashMap(Hash hasher_ = Hash()) :
        hasher_(hasher_),
        size_(0) {
        htable.resize(1);
    }

    template<typename Iter>
    HashMap(Iter start, Iter finish, Hash hasher_ = Hash()) :
        hasher_(hasher_),
        size_(0) {
        htable.resize(1);
        while (start != finish) {
            insert(*start);
            start++;
        }
    }

    HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> list_p, Hash hasher_ = Hash()) :
        HashMap(list_p.begin(), list_p.end(), hasher_) {}

    ~HashMap() {}


    void insert(const std::pair<const KeyType, ValueType> &pair_) {
        size_t pos = hasher_(pair_.first) % htable.size();
        for (const auto &e : htable[pos]) {
            if (e.first == pair_.first) {
                return;
            }
        }
        htable[pos].push_back(pair_);
        size_++;

        if (size_ * 2 >= htable.size()) {
            rehash(1);
        }
    }

    void rehash(bool InOrDescrease) {
        std::vector<std::pair<const KeyType, ValueType>> VecOfHash;
        for (const auto &i : htable) {
            for (const auto &e : i) {
                VecOfHash.push_back(e);
            }
        }

        size_t ReSize;
        if (InOrDescrease) {
            ReSize = htable.size() * 2;
        } else {
            ReSize = htable.size() / 2;
        }
        htable.clear();
        htable.resize(ReSize);
        size_ = 0;
        for (const auto &e : VecOfHash) {
            insert(e);
        }
    }

    void erase(const KeyType &key) {
        size_t pos = hasher_(key) % htable.size();
        for (auto e = htable[pos].begin(); e != htable[pos].end(); ++e) {
            if (e->first == key) {
                auto g = htable[pos].end();
                g--;
                std::vector<std::pair<KeyType, ValueType>> nv;
                while (g != e) {
                    nv.push_back(std::make_pair(g->first, g->second));
                    htable[pos].pop_back();
                    g = htable[pos].end();
                    g--;
                }
                htable[pos].pop_back();
                for (auto & e : nv) {
                    htable[pos].push_back(e);
                }
                size_--;

                if (size_ * 2 > htable.size()) {
                    rehash(0);
                }
                return;
            }
        }
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return (size_ == 0);
    }

    Hash hash_function() const {
        return hasher_;
    }

    HashMap &operator=(const HashMap hm) {
        htable.clear();
        size_ = 0;
        htable.resize(hm.htable.size());
        for (const auto & i : hm.htable) {
            for (const auto & e : i) {
                insert(e);
            }
        }
        return *this;
    }

    template<bool IsConstIt = true>
    class Iterator {

    private:
        typedef std::forward_iterator_tag                               iterator_category;
		typedef std::pair<const KeyType, ValueType>                     val_type;
		typedef std::ptrdiff_t                                          dif_type;

		typedef typename std::conditional<IsConstIt,
            const val_type*,
            val_type*>::type                                            pointer;

		typedef typename std::conditional<IsConstIt,
            const val_type&,
            val_type&>::type                                            refer;

        typedef typename std::conditional<IsConstIt,
                const HashMap<KeyType, ValueType, Hash>*,
                HashMap<KeyType, ValueType, Hash>*>::type               TypeOhHashMap;

        typedef typename std::conditional<IsConstIt,
                typename std::vector<val_type>::const_iterator,
                typename std::vector<val_type>::iterator>::type         TypeOfIter;


        TypeOhHashMap   hashmap_;
        TypeOfIter      it_;
        size_t          itpos;


    public:

        Iterator() {}

        Iterator(bool BeginOrEnd, TypeOhHashMap hashmap_) : hashmap_(hashmap_) {
            if (BeginOrEnd == 1) {
                itpos = 0;
                while (hashmap_->htable[itpos].empty()) {
                    if (itpos == hashmap_->htable.size() - 1) {
                        break;
                    }
                    itpos++;
                }
                it_ = hashmap_->htable[itpos].begin();
            } else {
                itpos = hashmap_->htable.size() - 1;
                it_ = hashmap_->htable[itpos].end();
            }

        }

        Iterator(const KeyType &key, TypeOhHashMap hashmap_) :
            hashmap_(hashmap_) {

            size_t pos = hashmap_->hasher_(key) % hashmap_->htable.size();
            for (auto e = hashmap_->htable[pos].begin(); e != hashmap_->htable[pos].end(); ++e) {
                if (e->first == key) {
                    itpos = pos;
                    it_ = e;
                    return;
                }
            }
            itpos = hashmap_->htable.size() - 1;
            it_ = hashmap_->htable[itpos].end();
        }


        Iterator &operator++() {
            it_++;
            if (itpos == hashmap_->htable.size() - 1) return *this;
            if (it_ == hashmap_->htable[itpos].end()) {
                itpos++;
                while (hashmap_->htable[itpos].empty()) {
                    if (itpos == hashmap_->htable.size() - 1) {
                        break;
                    }
                    itpos++;
                }
                it_ = hashmap_->htable[itpos].begin();
            }
            return *this;
        }

        Iterator operator++(int) {
            auto it2(*this);
            ++(*this);

            return it2;
        }

        friend bool operator==(const Iterator &lhs, const Iterator &rhs) {
            return (lhs.itpos == rhs.itpos && lhs.it_ == rhs.it_);
        }
        friend bool operator!=(const Iterator &lhs, const Iterator &rhs) {
            return !(lhs == rhs);
        }

        refer operator* () {
            return *it_;
        }

        pointer operator-> () {
            return &(*it_);
        }
    };

    typedef Iterator<false>   iterator;
    typedef Iterator<true>    const_iterator;

    iterator begin() {
        return iterator(true, this);
    }

    iterator end() {
        return iterator(false, this);
    }

    const_iterator begin() const {
        return const_iterator(true, this);
    }

    const_iterator end() const {
        return const_iterator(false, this);
    }

    iterator find(const KeyType &key) {
        return iterator(key, this);
    }

    const_iterator find(const KeyType &key) const {
        return const_iterator(key, this);
    }

    ValueType &operator[](const KeyType &key) {

        iterator it2 = iterator(key, this);
        if (it2 == this->end()) {
            this->insert(std::make_pair(key, ValueType()));
        }
        it2 = this->find(key);
        return it2->second;
    }

    const ValueType &at(const KeyType &key) const {
        const_iterator it2 = const_iterator(key, this);
        if (it2 == this->end()) {
            throw std::out_of_range("");
        }
        return it2->second;
    }

    void clear() {
        htable.clear();
        size_ = 0;
        htable.resize(1);
    }
};
