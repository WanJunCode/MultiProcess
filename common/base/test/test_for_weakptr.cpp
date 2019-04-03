

// class Person : public enable_shared_from_this<Person>
// {
//   public:
//     Person(const string &name)
//         : m_name{name}
//     {
//     }

//     ~Person()
//     {
//         cout << "release " << m_name << endl;
//     }

//     string getName() const
//     {
//         return m_name;
//     }

//     void setFather(shared_ptr<Person> f)
//     {
//         m_father = f;
//         if (f)
//         {
//             f->m_kids.push_back(shared_from_this());
//         }
//     }

//     void setMother(shared_ptr<Person> m)
//     {
//         m_mother = m;
//         if (m)
//         {
//             m->m_kids.push_back(shared_from_this());
//         }
//     }

//     shared_ptr<Person> getKid(size_t idx)
//     {
//         if (idx < m_kids.size())
//         {
//             weak_ptr<Person> p = m_kids.at(idx);
//             if (!p.expired())
//             {
//                 return p.lock();
//             }
//         }
//         return nullptr;
//     }

//   private:
//     string m_name;
//     shared_ptr<Person> m_father;
//     shared_ptr<Person> m_mother;
//     //vector<shared_ptr<Person>>    m_kids; // 循环依赖
//     vector<weak_ptr<Person>> m_kids;
// };

// // 测试代码
// shared_ptr<Person> jack{make_shared<Person>("Jack")};
// shared_ptr<Person> lucy{make_shared<Person>("Lucy")};
// shared_ptr<Person> john{make_shared<Person>("John")};
// john->setFather(jack);
// john->setMother(lucy);

// auto p = jack -> getKid(0);
// if (p)
// {
//     cout << p->getName() << endl;
// }