SUBKHANGULOV_SULTAN_https://official.contest.yandex.ru/contest/11654/run-report/19509348/

#include <vector>
#include <bits/stdc++.h>

using namespace std;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {

private:
    std::vector<std::vector<std::pair<const KeyType, ValueType>>> htable;
    Hash     hasher;
    size_t   sizer;


public:

    HashMap(const Hash _hasher = Hash()) :
        hasher(_hasher), sizer(0) {
        htable.resize(1);
    }

    template<typename Iter>
    HashMap(Iter _begin, Iter _end, Hash _hasher = Hash()) :
        hasher(_hasher), sizer(0) {

        htable.resize(1);
        while (_begin != _end) {
            this->insert(*_begin);
            _begin++;
        }
    }

    HashMap(const std::initializer_list<std::pair<KeyType, ValueType>> &list_p, Hash _hasher = Hash()) :
        HashMap(list_p.begin(), list_p.end(), _hasher) {}

    ~HashMap() {}


    void insert(const std::pair<const KeyType, ValueType> &_p) {

        size_t pos = hasher(_p.first) % htable.size();
        for (auto & e : htable[pos]) {
            if (e.first == _p.first) {
                return;
            }
        }
        htable[pos].push_back(_p);
        sizer++;

        if (sizer * 2 >= htable.size()) {
            std::vector<std::list<std::pair<const KeyType, ValueType>>> htable2;
            size_t ovser = htable.size() * 2;
            htable2.resize(ovser);

            for (auto & i : htable) {
                for (auto & e : i) {
                    size_t pos2 = hasher(e.first) % ovser;
                    htable2[pos2].push_back(e);
                }
            }
            htable.clear();
            htable.resize(ovser);
            for (auto & i : htable2) {
                for (auto & e : i) {
                    pos = hasher(e.first) % htable.size();
                    htable[pos].push_back(e);
                }
            }
        }

    }

    void erase(const KeyType &key) {

        size_t pos = hasher(key) % htable.size();
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
                sizer--;

                if (sizer * 2 < htable.size() > htable.size()) {
                    std::vector<std::list<std::pair<const KeyType, ValueType>>> htable2;
                    size_t ovser = htable.size() / 2;
                    htable2.resize(ovser);

                    for (auto & i : htable) {
                        for (auto & e : i) {
                            size_t pos2 = hasher(e.first) % htable2.size();
                            htable2[pos2].push_back(e);
                        }
                    }
                    htable.clear();
                    htable.resize(ovser);
                    for (auto & i : htable2) {
                        for (auto & e : i) {
                            pos = hasher(e.first) % htable.size();
                            htable[pos].push_back(e);
                        }
                    }
                }
                return;
            }
        }
    }

    size_t size() const {
        return sizer;
    }

    bool empty() const {
        return (sizer == 0);
    }

    Hash hash_function() const {
        return hasher;
    }

    HashMap & operator= (const HashMap hm) {

        htable.clear();
        sizer = 0;
        htable.resize(hm.htable.size());

        for (auto & i : hm.htable) {
            for (auto & e : i) {
                size_t pos = hasher(e.first) % htable.size();
                htable[pos].push_back(e);
            }
        }
        return *this;
    }

    //-------------------------------
    template<bool is_const_it = true>
    class Iterator {

    private:
        typedef forward_iterator_tag                               iterator_category;
		typedef pair<const KeyType, ValueType>                     val_type;
		typedef ptrdiff_t                                          dif_type;

		typedef typename conditional<is_const_it,
            const val_type*,
            val_type*>::type                                       pointer;

		typedef typename conditional<is_const_it,
            const val_type&,
            val_type&>::type                                       refer;

        typedef typename std::conditional<is_const_it,
                const HashMap<KeyType, ValueType, Hash>*,
                HashMap<KeyType, ValueType, Hash>*>::type          Tof_HashMap;

        typedef typename std::conditional<is_const_it,
                typename std::vector<val_type>::const_iterator,
                typename std::vector<val_type>::iterator>::type    Tof_Iter;


        Tof_HashMap   hm;
        Tof_Iter      it;
        size_t        nei;


    public:

        Iterator() {}

        Iterator(bool begin_or_end, Tof_HashMap _hm) : hm(_hm) {
            if (begin_or_end == 1) {
                nei = 0;
                while (hm->htable[nei].empty()) {
                    if (nei == hm->htable.size() - 1) {
                        break;
                    }
                    nei++;
                }
                it = hm->htable[nei].begin();
            } else {
                nei = hm->htable.size() - 1;
                it = hm->htable[nei].end();
            }

        }

        Iterator(const KeyType &key, Tof_HashMap _hm) :
            hm(_hm) {

            size_t pos = hm->hasher(key) % hm->htable.size();
            for (auto e = hm->htable[pos].begin(); e != hm->htable[pos].end(); ++e) {
                if (e->first == key) {
                    nei = pos;
                    it = e;
                    return;
                }
            }
            nei = hm->htable.size() - 1;
            it = hm->htable[nei].end();
        }


        Iterator & operator++ () {
            it++;
            if (nei == hm->htable.size() - 1) return *this;
            if (it == hm->htable[nei].end()) {
                nei++;
                while (hm->htable[nei].empty()) {
                    if (nei == hm->htable.size() - 1) {
                        break;
                    }
                    nei++;
                }
                it = hm->htable[nei].begin();
            }
            return *this;
        }

        Iterator operator++ (int) {
            auto it2(*this);
            ++(*this);

            return it2;
        }

        bool operator== (const Iterator &it2) const {
            return (nei == it2.nei && it == it2.it);
        }

        bool operator!= (const Iterator &it2) const {
            return (!(*this == it2));
        }

        refer operator* () {
            return *it;
        }

        pointer operator-> () {
            return &(*it);
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

    ValueType & operator[] (const KeyType &key) {

        iterator it2 = iterator(key, this);
        if (it2 == this->end()) {
            this->insert(std::make_pair(key, ValueType()));
        }
        it2 = this->find(key);
        return it2->second;
    }

    const ValueType & at(const KeyType &key) const {
        const_iterator it2 = const_iterator(key, this);
        if (it2 == this->end()) {
            throw std::out_of_range("");
        }
        return it2->second;
    }

    void clear() {
        htable.clear();
        sizer = 0;
        htable.resize(1);
    }
};
