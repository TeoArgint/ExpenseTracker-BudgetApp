#include "service/ExpenseService.h"
#include "validation/Validators.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>

ExpenseService::ExpenseService(IExpenseRepository& r, IBudgetStateRepository& s)
    : repo(r), stateRepo(s) {}

int ExpenseService::getYear(const std::string& d) const { return std::stoi(d.substr(0,4)); }
int ExpenseService::getMonth(const std::string& d) const { return std::stoi(d.substr(5,2)); }

double ExpenseService::totalSpentForStateMonth(const BudgetState& st) const {
    if (st.getYear() <= 0 || st.getMonth() <= 0) {
        return 0;
    }
    return totalSpentInMonth(st.getYear(), st.getMonth());
}

void ExpenseService::recalculateAvailable(BudgetState& st) const {
    const double available =
        st.getSalary() - st.getTotalFixedBills() - st.getMonthlySavingsContribution() -
        st.getTransferredToSavingsThisMonth() - totalSpentForStateMonth(st);
    st.setAvailable(available);
}

// CRUD
void ExpenseService::addExpense(int id,double a,const std::string& c,const std::string& d,const std::string& desc){
    Validators::validateId(id);
    Validators::validateAmount(a);
    Validators::validateCategory(c);
    Validators::isValidDateYYYYMMDD(d);
    Validators::validateDescription(desc);

    repo.add(Expense(id,a,c,d,desc));

    auto st = stateRepo.load();
    if(st.getYear()==getYear(d) && st.getMonth()==getMonth(d)){
        recalculateAvailable(st);
        stateRepo.save(st);
    }
}

void ExpenseService::updateExpense(int id,double a,const std::string& c,const std::string& d,const std::string& desc){
    Validators::validateId(id);
    Validators::validateAmount(a);
    Validators::validateCategory(c);
    Validators::isValidDateYYYYMMDD(d);
    Validators::validateDescription(desc);

    const Expense previous = repo.find(id);
    repo.update(Expense(id,a,c,d,desc));

    auto st = stateRepo.load();
    const bool oldInCurrentMonth = st.getYear() == getYear(previous.getDate()) && st.getMonth() == getMonth(previous.getDate());
    const bool newInCurrentMonth = st.getYear() == getYear(d) && st.getMonth() == getMonth(d);
    if (oldInCurrentMonth || newInCurrentMonth) {
        recalculateAvailable(st);
        stateRepo.save(st);
    }
}

void ExpenseService::deleteExpense(int id){
    const Expense previous = repo.find(id);
    repo.remove(id);

    auto st = stateRepo.load();
    if (st.getYear() == getYear(previous.getDate()) && st.getMonth() == getMonth(previous.getDate())) {
        recalculateAvailable(st);
        stateRepo.save(st);
    }
}

std::vector<Expense> ExpenseService::getAll() const { return repo.getAll(); }

// filters
std::vector<Expense> ExpenseService::filterByCategory(const std::string& c) const{
    std::vector<Expense> r;
    for(auto& e:repo.getAll())
        if(e.getCategory()==c) r.push_back(e);
    return r;
}

std::vector<Expense> ExpenseService::filterByDateRange(const std::string& s,const std::string& e) const{
    std::vector<Expense> r;
    for(auto& x:repo.getAll())
        if(x.getDate()>=s && x.getDate()<=e) r.push_back(x);
    return r;
}

std::vector<Expense> ExpenseService::filterByAmountRange(double a,double b) const{
    std::vector<Expense> r;
    for(auto& x:repo.getAll())
        if(x.getAmount()>=a && x.getAmount()<=b) r.push_back(x);
    return r;
}

// reports
std::map<std::string,double> ExpenseService::totalByCategory() const{
    std::map<std::string,double> m;
    for(auto& e:repo.getAll()) m[e.getCategory()]+=e.getAmount();
    return m;
}

std::map<std::string,double> ExpenseService::totalByMonth() const{
    std::map<std::string,double> m;
    for(auto& e:repo.getAll()) m[e.getDate().substr(0,7)]+=e.getAmount();
    return m;
}

// state
BudgetState ExpenseService::getState() const { return stateRepo.load(); }

void ExpenseService::initializeIfNeeded(int y,int m,double s){
    auto st=stateRepo.load();
    if(st.getYear()==0){
        st.setYear(y); st.setMonth(m);
        st.setSalary(s);
        recalculateAvailable(st);
        stateRepo.save(st);
        return;
    }

    if(st.getYear()!=y || st.getMonth()!=m){
        st.setYear(y);
        st.setMonth(m);
        if (st.getMonthlySavingsContribution() > 0) {
            st.setSavings(st.getSavings() + st.getMonthlySavingsContribution());
        }
        st.setTransferredToSavingsThisMonth(0);
    }
    recalculateAvailable(st);
    stateRepo.save(st);
}

void ExpenseService::setMonthlySalary(double s){
    Validators::validateAmount(s);
    auto st=stateRepo.load();
    st.setSalary(s);
    recalculateAvailable(st);
    stateRepo.save(st);
}

void ExpenseService::setMonthlySavingsContribution(double amount){
    if (amount < 0) {
        throw std::runtime_error("Monthly savings contribution cannot be negative.");
    }
    auto st=stateRepo.load();
    st.setMonthlySavingsContribution(amount);
    recalculateAvailable(st);
    stateRepo.save(st);
}

void ExpenseService::addFixedBill(const std::string& name, double amount){
    if (name.empty()) {
        throw std::runtime_error("Fixed bill name cannot be empty.");
    }
    Validators::validateAmount(amount);
    auto st = stateRepo.load();
    st.addFixedBill(name, amount);
    recalculateAvailable(st);
    stateRepo.save(st);
}

void ExpenseService::removeFixedBill(const std::string& name){
    auto st = stateRepo.load();
    st.removeFixedBill(name);
    recalculateAvailable(st);
    stateRepo.save(st);
}

void ExpenseService::addGoal(const std::string& name, double target){
    if (name.empty()) {
        throw std::runtime_error("Goal name cannot be empty.");
    }
    Validators::validateAmount(target);
    auto st = stateRepo.load();
    st.addGoal(name, target);
    stateRepo.save(st);
}

void ExpenseService::removeGoal(const std::string& name){
    auto st = stateRepo.load();
    st.removeGoal(name);
    stateRepo.save(st);
}

void ExpenseService::markGoalNotified(const std::string& name, bool notified){
    auto st = stateRepo.load();
    st.markGoalNotified(name, notified);
    stateRepo.save(st);
}

void ExpenseService::transferToSavings(double x){
    Validators::validateAmount(x);
    auto st=stateRepo.load();
    if (x > st.getAvailable()) {
        throw std::runtime_error("Not enough available money for savings transfer.");
    }
    st.setSavings(st.getSavings()+x);
    st.setTransferredToSavingsThisMonth(st.getTransferredToSavingsThisMonth() + x);
    recalculateAvailable(st);
    stateRepo.save(st);
}

// totals
double ExpenseService::totalSpentInMonth(int y,int m) const{
    double s=0;
    for(auto& e:repo.getAll())
        if(getYear(e.getDate())==y && getMonth(e.getDate())==m)
            s+=e.getAmount();
    return s;
}

double ExpenseService::totalSpentInYear(int y) const{
    double s=0;
    for(auto& e:repo.getAll())
        if(getYear(e.getDate())==y) s+=e.getAmount();
    return s;
}

// estimate
int ExpenseService::monthsToAfford(double t) const{
    if (t <= 0) {
        throw std::runtime_error("Target amount must be greater than 0.");
    }
    auto st=stateRepo.load();
    const double currentFunds = std::max(0.0, st.getAvailable()) + st.getSavings();
    if (currentFunds >= t) {
        return 0;
    }

    const double monthlyCapacity = st.getSalary() - st.getTotalFixedBills() - st.getMonthlySavingsContribution();
    if(monthlyCapacity<=0) {
        return -1;
    }
    return static_cast<int>(std::ceil((t - currentFunds) / monthlyCapacity));
}

// extras
std::vector<std::pair<std::string,double>> ExpenseService::topCategories() const{
    auto m=totalByCategory();
    std::vector<std::pair<std::string,double>> v(m.begin(),m.end());
    std::sort(v.begin(),v.end(),[](auto&a,auto&b){return a.second>b.second;});
    return v;
}

std::map<std::string,double> ExpenseService::graphData() const{
    return totalByCategory();
}

void ExpenseService::exportCSV(const std::string& f) const{
    std::ofstream fout(f);
    fout<<"id,amount,category,date,desc\n";
    for(auto& e:repo.getAll())
        fout<<e.getId()<<","<<e.getAmount()<<","<<e.getCategory()
            <<","<<e.getDate()<<","<<e.getDescription()<<"\n";
}
