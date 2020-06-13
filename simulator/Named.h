// Helper classes for named arguments
template<typename T>
class Named {
    T value;
public:
    explicit Named(T value): value{value} {}
    operator T() const {return value;}
};

class NumTasks: public Named<size_t> {
    using Named<size_t>::Named;
};

class NumThreads: public Named<int> {
    using Named<int>::Named;
};