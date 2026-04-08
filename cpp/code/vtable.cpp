#include <iostream>

struct VTable {
    void (*speak)(void*);
    void (*move)(void*);
};

void dog_speak(void* self) { std::cout << "wofff" << '\n'; }

void dog_move(void* self) { std::cout << "Dog movesss" << '\n'; }

void cat_speak(void* self) { std::cout << "meowww" << '\n'; }

void cat_move(void* self) { std::cout << "Cat Moves" << '\n'; }

VTable dog_vtable = {dog_speak, dog_move};

VTable cat_vtable = {cat_speak, cat_move};

struct Animal {
    VTable* vptr;
};

int main() {
    Animal dog{&dog_vtable};
    Animal cat{&cat_vtable};

    dog.vptr->speak(&dog);  // "Woof"
    cat.vptr->speak(&cat);  // "Meow"

    return 0;
}
