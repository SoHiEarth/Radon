#ifndef OBJECT_H
class Object {
  public:
    bool is_static = false;
    virtual void Init() {};
    virtual void Update() {};
    virtual void Render() {};
    virtual void Quit() {};
};
#endif // OBJECT_H
