```
#include <chaiscript/chaiscript.hpp>
#include <iostream>

struct foo {
    float myFloat = 5.f;
    bool bar(float value) {
        std::cout << "C++ bar aufgerufen mit: " << value << std::endl;
        return value > myFloat;
    }
};

int main() {
    chaiscript::ChaiScript chai;

    // 1. Typ registrieren
    chai.add(chaiscript::user_type<foo>(), "foo");

    // 2. Konstruktor registrieren (damit 'var f = foo()' im Skript geht)
    chai.add(chaiscript::constructor<foo()>(), "foo");

    // 3. Datenmember registrieren (Variable)
    chai.add(chaiscript::fun(&foo::myFloat), "myFloat");

    // 4. Member-Funktion registrieren (Methode)
    chai.add(chaiscript::fun(&foo::bar), "bar");

    try {
        chai.eval(R"(
            var f = foo()              // C++ Objekt erstellen
            print("Startwert: ${f.myFloat}")

            f.myFloat = 10.0           // Wert ändern
            var result = f.bar(15.5)   // Methode aufrufen

            print("Ergebnis: ${result}")
        )");
    } catch (const std::exception &e) {
        std::cerr << "Fehler: " << e.what() << std::endl;
    }

    return 0;
}

```
