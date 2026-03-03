#include "repo/InMemoryExpenseRepository.h"
#include <cassert>

int main() {
    InMemoryExpenseRepository r;
//cases in wich the tests are valid and work
    r.add(Expense("11", 10.5, "food", "2026-03-01", "pizza"));
    assert(r.getAll().size() == 1);
    assert(r.findById("11").getAmount() == 10.5);

    r.update(Expense("21", 20.0, "food", "2026-03-01", "pastas"));
    assert(r.findById("21").getAmount() == 20.0);
    assert(r.findById("21").getDate() == "2026-03-01");

    r.remove("11");
    assert(r.getAll().empty());

//cases in wich the test are not valid and return a error
    r.add(Expense("143", 200, "bills ","2026-02-11", "electricity"));

    return 0;
}