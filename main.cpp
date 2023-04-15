#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>

#define STRING_OF(x) #x

using namespace std;

const string SEPERATOR = "---\n";
const string EMPLOYEE_FILE_NAME = "/employees.csv";
const string TEAMS_FILE_NAME = "/team.csv";
const string SALARY_CONFIGS_FILE_NAME = "/salary_configs.csv";
const string WORKING_HOURS_FILE_NAME = "/working_hours.csv";
const string REPORT_SALARIES = "report_salaries";
const string REPORT_EMPLOYEE_SALARY = "report_employee_salary";
const string REPORT_TEAM_SALARY = "report_team_salary";
const string REPORT_TOTAL_HOURS_PER_DAY = "report_total_hours_per_day";
const string REPORT_EMPLOYEE_PER_HOUR = "report_employee_per_hour";
const string SHOW_SALARY_CONFIG = "show_salary_config";
const string UPDATE_SALARY_CONFIG = "update_salary_config";
const string ADD_WORKING_HOURS = "add_working_hours";
const string DELETE_WORKING_HOURS = "delete_working_hours";
const string UPDATE_TEAM_BONUS = "update_team_bonus";
const string FIND_TEAM_FOR_BONUS = "find_teams_for_bonus";
const int DAYS_IN_MONTH = 30;
const int HOURS_IN_DAY = 24;
const int INF = 10e8 + 5;
const int START_HOUR = 0;
const int FINISH_HOUR = 23;
const int START_DAY = 1;
const int NO_CHANGE = -1;
const bool ACTIVE = true;
const bool INACTIVE = false;
const int ACTIVITY_STATUS = 24;

enum error_codes
{
    EMPLOYEE_NOT_FOUND,
    TEAM_NOT_FOUND,
    INVALID_ARGUEMENTS,
    INVALID_LEVEL,
    INVALID_INTERVAL,
    NO_BONUS_TEAMS,
    UNKNOWN_ERROR
};

enum levels
{
    junior,
    expert,
    senior,
    team_lead
};

void throw_an_error(error_codes code)
{
    switch (code)
    {
    case EMPLOYEE_NOT_FOUND:
        cout << STRING_OF(EMPLOYEE_NOT_FOUND);
        break;
    case TEAM_NOT_FOUND:
        cout << STRING_OF(TEAM_NOT_FOUND);
        break;
    case INVALID_ARGUEMENTS:
        cout << STRING_OF(INVALID_ARGUEMENTS);
        break;
    case INVALID_LEVEL:
        cout << STRING_OF(INVALID_LEVEL);
        break;
    case INVALID_INTERVAL:
        cout << STRING_OF(INVALID_INTERVAL);
        break;
    case NO_BONUS_TEAMS:
        cout << STRING_OF(NO_BONUS_TEAMS);
        break;
    default:
        cout << STRING_OF(UNKNOWN_ERROR);
        break;
    }
    cout << endl;
}
class Level
{
private:
    string name;
    int base_salary;
    int salary_per_hour;
    int salary_per_extra_hour;
    int official_working_hour;
    int tax_percentage;

public:
    Level(const Level &source);
    Level(string _name, int _base_salary, int _salary_per_hour,
          int _salary_per_extra_hour, int _official_working_hour, int _tax_percentage);
    void show_level_config() const;
    void update_level_config(int new_base_salary, int new_salary_per_hour, int new_salary_per_extra_hour,
                             int new_official_working_hours, int new_tax_percentage);
    string get_name() { return name; }
    int get_base_salary() { return base_salary; }
    int get_salary_per_hour() { return salary_per_hour; }
    int get_salary_per_extra_hour() { return salary_per_extra_hour; }
    int get_official_working_hour() { return official_working_hour; }
    int get_tax_percentage() { return tax_percentage; }
};
Level::Level(const Level &source)
    : name{source.name}, base_salary{source.base_salary}, salary_per_hour{source.salary_per_hour},
      salary_per_extra_hour{source.salary_per_extra_hour}, official_working_hour{source.official_working_hour}, tax_percentage{source.tax_percentage}
{
}

Level::Level(string _name, int _base_salary, int _salary_per_hour,
             int _salary_per_extra_hour, int _official_working_hour, int _tax_percentage)
    : name{_name}, base_salary{_base_salary}, salary_per_hour{_salary_per_hour},
      salary_per_extra_hour{salary_per_extra_hour}, official_working_hour{_official_working_hour},
      tax_percentage{_tax_percentage}
{
}
void Level::show_level_config() const
{
    cout << "Base Salary: " << this->base_salary << endl;
    cout << "Salary Per Hour: " << this->salary_per_hour << endl;
    cout << "Salary Per Extra Hour: " << this->salary_per_extra_hour << endl;
    cout << "Official Working Hours: " << this->official_working_hour << endl;
    cout << "Tax: " << this->tax_percentage << '%' << endl;
}
void Level::update_level_config(int new_base_salary, int new_salary_per_hour, int new_salary_per_extra_hour,
                                int new_official_working_hours, int new_tax_percentage)
{
    if (new_base_salary != NO_CHANGE)
    {
        base_salary = new_base_salary;
    }
    if (new_salary_per_hour != NO_CHANGE)
    {
        salary_per_hour = new_salary_per_hour;
    }
    if (new_salary_per_extra_hour != NO_CHANGE)
    {
        salary_per_extra_hour = new_salary_per_extra_hour;
    }
    if (new_official_working_hours != NO_CHANGE)
    {
        official_working_hour = new_official_working_hours;
    }
    if (new_tax_percentage != NO_CHANGE)
    {
        tax_percentage = new_tax_percentage;
    }
}
class Salary
{
private:
    int working_hours;
    int bonus_salary;
    int total_earning;
    int tax;
    int salary;

public:
    Salary();
    int get_working_hours() const { return working_hours; }
    int get_bonus_salary() const { return bonus_salary; }
    int get_total_earning() const { return total_earning; }
    int get_tax() const { return tax; }
    int get_salary() const { return salary; }
    void add_working_hours(int hours) { working_hours += hours; }
    void update_bonus_salary(int new_bonus_salary) { bonus_salary = new_bonus_salary; }
    void update_total_earning() { total_earning = salary + bonus_salary - tax; }
    void update_tax(int tax_percentage) { (tax = salary * tax_percentage) / 100 + (bonus_salary * tax_percentage) / 100; }
    void update_salary(int new_salary) { salary = new_salary; }
};
Salary::Salary()
    : working_hours{0}, bonus_salary{0}, total_earning{0}, tax{0}, salary{0}
{
}
class Employee
{
private:
    int id;
    int team_id;
    string name;
    int age;
    Salary *salary_info;
    int inactive_days;
    bool work_status[DAYS_IN_MONTH + 1][HOURS_IN_DAY + 1];
    Level *expertise;
    void update_salary_info()
    {
        int official_working_hours = expertise->get_official_working_hour();
        int salary_per_hour = expertise->get_salary_per_hour();
        int salary_per_extra_hour = expertise->get_salary_per_extra_hour();
        int tax_percentage = expertise->get_tax_percentage();
        int working_hours = salary_info->get_working_hours();
        int salary = salary_info->get_bonus_salary();
        if (working_hours <= official_working_hours)
        {
            salary += working_hours * salary_per_hour;
        }
        else
        {
            salary += expertise->get_base_salary() + (working_hours - official_working_hours) * salary_per_extra_hour;
        }
        int tax = tax_percentage * salary / 100;
        int total_earning = salary - tax;
        salary_info->update_salary(salary);
        salary_info->update_tax(tax);
        salary_info->update_total_earning();
    }
    bool there_is_overlap(int day, int start_hour, int finish_hour) const
    {
        for (int i = start_hour; i <= finish_hour; i++)
        {
            if (work_status[day][i] == ACTIVE)
            {
                return true;
            }
        }
        return false;
    }
    void add_working_hours(int hours)
    {
        salary_info->add_working_hours(hours);
        update_salary_info();
    }

public:
    Employee(int _id, string _name, int _age, Level *_level);
    Employee();
    Employee(const Employee &old_employee);
    int get_id() const { return id; }
    string get_name() const { return name; }
    int get_salary() const { return salary_info->get_salary(); };
    int get_total_earning() const { return salary_info->get_total_earning(); }
    int get_working_hours() const { return salary_info->get_working_hours(); }
    void show_employee_summary() const;
    void show_employee_full_info() const;
    void add_working_hours(int day, int start_hour, int finish_hour);
    void delete_working_hours(int day);
    void update_bonus_salary(int bonus_salary);
};
Employee::Employee()
{
} // Just for creating null object
Employee::Employee(int _id, string _name, int _age, Level *_level)
    : id{_id}, name{_name}, age{_age}, expertise{_level}
{
    for (int i = 1; i <= DAYS_IN_MONTH; i++)
    {
        for (int j = 0; j < HOURS_IN_DAY; j++)
            work_status[i][j] = INACTIVE;
    }
}
Employee::Employee(const Employee &source)
    : name{source.name}, id{source.id}, team_id{source.team_id}, inactive_days{source.inactive_days}
{
    this->expertise = new Level(*(source.expertise));
}
void Employee::update_bonus_salary(int bonus_salary)
{
    this->salary_info->update_bonus_salary(bonus_salary);
    this->salary_info->update_tax(this->expertise->get_tax_percentage());
    this->salary_info->update_total_earning();
}

void Employee::show_employee_summary() const
{
    cout << "ID: " << this->id << endl;
    cout << "Name: " << this->name << endl;
    cout << "Total Working Hours: " << this->salary_info->get_working_hours() << endl;
    cout << "Total Earning: " << this->salary_info->get_total_earning() << endl;
}

void Employee::show_employee_full_info() const
{
    cout << "ID: " << this->id << endl;
    cout << "Name: " << this->name << endl;
    cout << "Age: " << this->name << endl;
    cout << "Level: " << this->expertise->get_name() << endl;
    cout << "Team ID: " << this->team_id << endl;
    cout << "Total Working Hours: " << this->salary_info->get_working_hours() << endl;
    cout << "Absent days: " << this->inactive_days << endl;
    cout << "Salary: " << this->salary_info->get_salary() << endl;
    cout << "Bonus: " << this->salary_info->get_bonus_salary() << endl;
    cout << "Tax: " << this->salary_info->get_tax() << endl;
    cout << "Total earning: " << this->salary_info->get_total_earning() << endl;
}

void Employee::add_working_hours(int day, int start_hour, int finish_hour)
{
    if (there_is_overlap(day, start_hour, finish_hour))
    {
        throw_an_error(INVALID_INTERVAL);
        return;
    }
    if (work_status[day][ACTIVITY_STATUS] == INACTIVE)
    {
        this->inactive_days -= 1;
    }
    for (int i = start_hour; i <= finish_hour; i++)
    {
        work_status[day][i] = ACTIVE;
    }
    this->add_working_hours(finish_hour - start_hour);
}

void Employee::delete_working_hours(int day)
{
    int reduced_hours = 0;
    for (int i = START_HOUR; i <= HOURS_IN_DAY; i++)
    {
        if (work_status[day][i])
        {
            reduced_hours++;
        }
        work_status[day][i] = INACTIVE;
    }
    work_status[day][ACTIVITY_STATUS] = INACTIVE;
    this->add_working_hours(reduced_hours * -1);
}

class Team
{
private:
    int id;
    int leader;
    vector<Employee *> members;
    int bonus_percentage;
    int bonus_min_working_hours;
    int bonus_max_working_hours_variance;
    int total_earning;
    bool eligible_for_bonus = false;

public:
    Team(int _id, int _leader, vector<Employee *> _members,
         int _bonus_min_working_hours, int _bonus_max_working_hours_variance);
    Team();
    int get_id() const { return id; }
    void show_team_info() const;
    int get_total_working_hours() const;
    void update_bonus_percentage(int new_bonus_percentage);
    int get_average_working_hours() const;
    int get_variance_working_hours(int &total_working_hours) const;
    void set_eligible_for_bonus();
    bool is_eligible_for_bonus() { return eligible_for_bonus; }
};

Team::Team(int _id, int _leader, vector<Employee *> _members,
           int _bonus_min_working_hours, int _bonus_max_working_hours_variance)
    : id{_id}, leader{_leader}, bonus_min_working_hours{_bonus_min_working_hours},
      bonus_max_working_hours_variance{_bonus_max_working_hours_variance}
{
}

void Team::show_team_info() const
{
    cout << "ID: " << this->id << endl;
    cout << "Head ID: " << this->leader << endl;
    cout << "Head Name: " << this->members[0]->get_name() << endl;
    int total_working_hours = this->get_total_working_hours();
    cout << "Total Working Hours: " << total_working_hours << endl;
    cout << setprecision(1) << "Average Working Hours: " << total_working_hours / (this->members.size() * 1.0) << endl;
    cout << "Bonus: " << this->bonus_percentage << endl;
    cout << SEPERATOR;
    for (auto &employee : this->members)
    {
        cout << "Member ID: " << employee->get_id() << endl;
        cout << "Total Earning: " << employee->get_total_earning() << endl;
        cout << SEPERATOR;
    }
}

void Team::update_bonus_percentage(int new_bonus_percentage)
{
    this->bonus_percentage = new_bonus_percentage;
    for (auto &employee : this->members)
    {
        int employee_salary = employee->get_salary();
        employee->update_bonus_salary(employee_salary * new_bonus_percentage);
    }
}

int Team::get_total_working_hours() const
{
    int total_working_hours = 0;
    for (auto &employee : this->members)
    {
        total_working_hours += employee->get_working_hours();
    }
    return total_working_hours;
}

int Team::get_average_working_hours() const
{
    return get_total_working_hours() / this->members.size() * 1.0;
}

int Team::get_variance_working_hours(int &total_working_hours) const
{
    int variance_working_hours = 0;
    int average_working_hours = this->get_average_working_hours();
    total_working_hours *= this->members.size();
    for (auto &employee : this->members)
    {
        int variance = (average_working_hours - employee->get_working_hours());
        variance *= variance;
        variance /= this->members.size();
        variance_working_hours += variance;
    }
    return variance_working_hours;
}

void Team::set_eligible_for_bonus()
{
    int total_working_hours = 0;
    int variance_working_hours = this->get_variance_working_hours(total_working_hours);
    if (variance_working_hours <= this->bonus_max_working_hours_variance && total_working_hours >= bonus_min_working_hours)
    {
        eligible_for_bonus = true;
    }
}

class SalaryService
{
private:
    vector<Employee *> employees;
    vector<Team *> teams;
    vector<Level *> levels;
    vector<vector<int>> timetable;
    Employee *find_employee(int id) const
    {
        for (auto &employee : employees)
        {
            if (employee->get_id() == id)
            {
                return employee;
            }
        }
        return NULL;
    }

    Team *find_team(int id) const
    {
        for (auto &team : teams)
        {
            if (team->get_id() == id)
            {
                return team;
            }
        }
        return NULL;
    }
    int find_working_hours_today(int today) const
    {
        int working_hours = 0;
        for (int hour = START_HOUR; hour <= FINISH_HOUR; hour++)
        {
            working_hours += timetable[today][hour];
        }
    }

    template <typename type>
    vector<int> find_max_working(vector<pair<type, int>> working_info) const
    {
        vector<int> max_working;
        for (int day = 0; day < working_info.size(); day++)
        {
            if (working_info[day].first == working_info[0].first)
            {
                max_working.push_back(working_info[day].second);
            }
            else
            {
                break;
            }
        }
        sort(max_working.begin(), max_working.end());
        return max_working;
    }

    template <typename type>
    vector<int> find_min_working(vector<pair<type, int>> working_info) const
    {
        vector<int> min_working;
        for (int day = working_info.size() - 1; day >= 0; day--)
        {
            if (working_info[day].first == working_info[working_info.size() - 1].first)
            {
                min_working.push_back(working_info[day].second);
            }
            else
            {
                break;
            }
        }
        sort(min_working.begin(), min_working.end());
        return min_working;
    }

    float find_workers_this_time(int hour) const
    {
        float average_workers = 0;
        for (int day = START_DAY; day <= DAYS_IN_MONTH; day++)
        {
            average_workers += timetable[day][hour];
        }
        return average_workers / DAYS_IN_MONTH;
    }

    Level *find_level_config(string level) const
    {
        if (level == STRING_OF(junior))
        {
            return levels[junior];
        }
        else if (level == STRING_OF(senior))
        {
            return levels[senior];
        }
        else if (level == STRING_OF(team_lead))
        {
            return levels[team_lead];
        }
        else if (level == STRING_OF(expert))
        {
            return levels[expert];
        }
        else
        {
            return NULL;
        }
    }
    vector<string> split_by_commas(string str)
    {
        vector<string> result;
        int start_pos = 0;
        while (start_pos < str.length())
        {
            int comma_pos = str.find(",", start_pos);
            if (comma_pos == string::npos)
            {
                result.push_back(str.substr(start_pos));
                break;
            }
            result.push_back(str.substr(start_pos, comma_pos - start_pos));
            start_pos = comma_pos + 1;
        }
        return result;
    }
    vector<int> split_by_dollarsign(string str)
    {
        vector<int> result;
        int start_pos = 0;
        while (start_pos < str.length())
        {
            int comma_pos = str.find("$", start_pos);
            if (comma_pos == string::npos)
            {
                result.push_back(stoi(str.substr(start_pos)));
                break;
            }
            result.push_back(stoi(str.substr(start_pos, comma_pos - start_pos)));
            start_pos = comma_pos + 1;
        }
        return result;
    }

    void read_employee(string str)
    {
        vector<string> fields = split_by_commas(str);
        Level *expertise = this->find_level_config(fields[3]);
        employees.push_back(new Employee(stoi(fields[0]), fields[1], stoi(fields[2]), expertise)); // here needs edit
    }
    void read_level_config(string str)
    {
        vector<string> fields = split_by_commas(str);
        levels.push_back(new Level(fields[0], stoi(fields[1]), stoi(fields[2]), stoi(fields[3]), stoi(fields[4]), stoi(fields[5])));
    }

    void read_working_hours(string str)
    {
        int _employee_id, _day, _start_time, _finish_time;
        char delimiter;
        stringstream line(str);
        line >> _employee_id >> delimiter >> _day >> delimiter >> _start_time >> delimiter >> _finish_time;
        add_working_hours(_employee_id, _day, _start_time, _finish_time);
    }
    void read_teams_info(string str)
    {
        vector<string> fields = split_by_commas(str);
        vector<int> members_id = split_by_dollarsign(fields[2]);
        vector<Employee *> members;
        for (auto id : members_id)
        {
            members.push_back(find_employee(id));
        }
        teams.push_back(new Team(stoi(fields[0]), stoi(fields[1]), members, stoi(fields[3]), stoi(fields[4])));
    }

public:
    SalaryService();
    void report_salaries() const;
    void report_employee_salary(int employee_id) const;
    void report_team_salary(int team_id) const;
    void report_total_hours_per_day(int start_day, int finish_day) const;
    void report_employee_per_hour(int start_hour, int finish_hour) const;
    void show_salary_config(string level) const;
    void update_salary_config(string level, int new_base_salary, int new_salary_per_hour,
                              int new_salary_per_extra_hour, int new_official_working_hours, int new_tax);
    void add_working_hours(int employee_id, int day, int start_hour, int finish_hour);
    void delete_working_horus(int employee_id, int day);
    void update_team_bonus(int team_id, int new_bonus_percentage);
    void find_teams_for_bonus() const;
    void get_information_from_csv_file();
    void add_level_config_info();
    void add_employee_info();
    void add_teams_info();
    void add_working_hours_info();
};

void SalaryService::get_information_from_csv_file()
{
    add_level_config_info();
    add_employee_info();
    add_teams_info();
    add_working_hours_info();
}

void SalaryService::add_level_config_info()
{
    ifstream input_csv(SALARY_CONFIGS_FILE_NAME);

    string line;
    getline(input_csv, line);

    while (getline(input_csv, line))
    {
        cout << line << endl;
        read_level_config(line);
    }

    input_csv.close();
}

void SalaryService::add_employee_info()
{
    ifstream input_csv(EMPLOYEE_FILE_NAME);
    string line;
    getline(input_csv, line);

    while (getline(input_csv, line))
    {
        cout << line << endl;
        read_employee(line);
    }

    input_csv.close();
}

void SalaryService::add_teams_info()
{
    ifstream input_csv(EMPLOYEE_FILE_NAME);

    string line;
    getline(input_csv, line);

    while (getline(input_csv, line))
    {
        cout << line << endl;
        read_teams_info(line);
    }

    input_csv.close();
}

void SalaryService::add_working_hours_info()
{
    ifstream input_csv(WORKING_HOURS_FILE_NAME);
    string line;
    getline(input_csv, line);
    while (getline(input_csv, line))
    {
        cout << line << endl;
        read_working_hours(line);
    }
    input_csv.close();
}

SalaryService::SalaryService()
{
    vector<vector<int>> timetable(DAYS_IN_MONTH + 1, vector<int>(HOURS_IN_DAY, 1));
    get_information_from_csv_file();
}

void SalaryService::report_salaries() const
{
    for (auto &employee : employees)
    {
        employee->show_employee_summary();
        cout << SEPERATOR;
    }
}

void SalaryService::report_employee_salary(int employee_id) const
{
    Employee *chosen_employee = find_employee(employee_id);
    if (chosen_employee == NULL)
    {
        throw_an_error(EMPLOYEE_NOT_FOUND);
        return;
    }
    chosen_employee->show_employee_full_info();
}

void SalaryService::report_team_salary(int team_id) const
{
    Team *chosen_team = find_team(team_id);
    if (chosen_team == NULL)
    {
        throw_an_error(TEAM_NOT_FOUND);
        return;
    }
    chosen_team->show_team_info();
}

void SalaryService::report_total_hours_per_day(int start_day, int finish_day) const
{
    if (start_day < 0 || finish_day > 30 || start_day > finish_day)
    {
        throw_an_error(INVALID_ARGUEMENTS);
        return;
    }
    vector<pair<int, int>> working_per_day;
    for (int day = start_day; day <= finish_day; day++)
    {
        int working_hours = find_working_hours_today(day);
        working_per_day.push_back(make_pair(working_hours, day));
        cout << "Day #:" << day << ": " << working_hours << endl;
    }
    cout << SEPERATOR;
    sort(working_per_day.begin(), working_per_day.end());
    vector<int> max_working_days = find_max_working(working_per_day);
    vector<int> min_working_days = find_min_working(working_per_day);
    cout << "Day(s) with Max Working Hours: ";
    for (int i = 0; i < max_working_days.size(); i++)
    {
        cout << max_working_days[i] << ' ';
    }
    cout << "Day(s) with Min Working Hours: ";
    for (int i = 0; i < min_working_days.size(); i++)
    {
        cout << min_working_days[i] << ' ';
    }
}

void SalaryService::report_employee_per_hour(int start_hour, int finish_hour) const
{
    if (start_hour < 0 || finish_hour > 30 || start_hour > finish_hour)
    {
        throw_an_error(INVALID_ARGUEMENTS);
        return;
    }
    vector<pair<float, int>> worker_per_hour;
    for (int hour = start_hour; hour <= finish_hour; hour++)
    {
        float average_workers = find_workers_this_time(hour);
        worker_per_hour.push_back(make_pair(average_workers, hour)); // i think this is wrong . arguments placed instead of each other.
        cout << hour << "-" << hour + 1 << ": " << setprecision(1) << average_workers;
    }
    cout << SEPERATOR;
    sort(worker_per_hour.begin(), worker_per_hour.end());
    vector<int> max_working_interval = find_max_working(worker_per_hour);
    vector<int> min_working_interval = find_min_working(worker_per_hour);
    cout << "Period(s) with Max Working Employees: ";
    for (int i = 0; i < max_working_interval.size(); i++)
    {
        cout << max_working_interval[i] << '-' << max_working_interval[i] + 1 << ' ';
    }
    cout << "Period(s) with Min Working Employees: ";
    for (int i = 0; i < min_working_interval.size(); i++)
    {
        cout << min_working_interval[i] << '-' << min_working_interval[i] + 1 << ' ';
    }
}

void SalaryService::show_salary_config(string level) const
{
    Level *chosen_level = find_level_config(level);
    if (chosen_level == NULL)
    {
        throw_an_error(INVALID_LEVEL);
        return;
    }
    chosen_level->show_level_config();
}

void SalaryService::update_salary_config(string level, int new_base_salary, int new_salary_per_hour,
                                         int new_salary_per_extra_hour, int new_official_working_hours, int new_tax)
{
    Level *chosen_level = find_level_config(level);
    if (chosen_level == NULL)
    {
        throw_an_error(INVALID_LEVEL);
        return;
    }
    chosen_level->update_level_config(new_base_salary, new_salary_per_hour,
                                      new_salary_per_extra_hour, new_official_working_hours, new_tax);
}

void SalaryService::add_working_hours(int employee_id, int day, int start_hour, int finish_hour)
{
    Employee *chosen_employee = find_employee(employee_id);
    if (chosen_employee == NULL)
    {
        throw_an_error(EMPLOYEE_NOT_FOUND);
        return;
    }
    if (day < 0 || day > 30 ||
        start_hour < 0 || start_hour > 23 ||
        finish_hour < 1 || finish_hour > 24 ||
        finish_hour - start_hour <= 0)
    {
        throw_an_error(INVALID_ARGUEMENTS);
        return;
    }
    chosen_employee->add_working_hours(day, start_hour, finish_hour);
}

void SalaryService::delete_working_horus(int employee_id, int day)
{
    Employee *chosen_employee = find_employee(employee_id);
    if (chosen_employee == NULL)
    {
        throw_an_error(EMPLOYEE_NOT_FOUND);
        return;
    }
    if (day < 0 || day > 30)
    {
        throw_an_error(INVALID_ARGUEMENTS);
        return;
    }
    chosen_employee->delete_working_hours(day);
}

void SalaryService::update_team_bonus(int team_id, int new_bonus_percentage)
{
    Team *chosen_team = find_team(team_id);
    if (chosen_team == NULL)
    {
        throw_an_error(TEAM_NOT_FOUND);
        return;
    }
    if (new_bonus_percentage < 0 || new_bonus_percentage > 100)
    {
        throw_an_error(INVALID_ARGUEMENTS);
        return;
    }
    chosen_team->update_bonus_percentage(new_bonus_percentage);
}

void SalaryService::find_teams_for_bonus() const
{
    vector<Team *> eligible_teams;
    for (auto &team : teams)
    {
        if (team->is_eligible_for_bonus())
        {
            eligible_teams.push_back(team);
        }
    }
    if (!(eligible_teams.size() > 0))
        throw_an_error(NO_BONUS_TEAMS);
    return;
    for (auto &team : eligible_teams)
    {
        cout << "Team ID: " << team->get_id() << endl;
    }
}

int main()
{
    SalaryService salary_manager;
    string command;
    while (cin >> command)
    {
        if (command == REPORT_SALARIES)
        {
            salary_manager.report_salaries();
        }
        else if (command == REPORT_TEAM_SALARY)
        {
            int team_id;
            cin >> team_id;
            salary_manager.report_team_salary(team_id);
        }
        else if (command == REPORT_EMPLOYEE_SALARY)
        {
            int employee_id;
            cin >> employee_id;
            salary_manager.report_employee_salary(employee_id);
        }
        else if (command == REPORT_TOTAL_HOURS_PER_DAY)
        {
            int start_day, finish_day;
            cin >> start_day >> finish_day;
            salary_manager.report_total_hours_per_day(start_day, finish_day);
        }
        else if (command == REPORT_EMPLOYEE_PER_HOUR)
        {
            int start_hour, finish_hour;
            cin >> start_hour >> finish_hour;
            salary_manager.report_employee_per_hour(start_hour, finish_hour);
        }
        else if (command == SHOW_SALARY_CONFIG)
        {
            string level;
            salary_manager.show_salary_config(level);
        }
        else if (command == UPDATE_SALARY_CONFIG)
        {
            string level;
            int new_base_salary, new_salary_per_hour, new_salary_per_extra_hour, new_official_working_hours, new_tax_percentage;
            cin >> level >> new_base_salary, new_salary_per_hour >> new_salary_per_extra_hour >> new_official_working_hours >> new_tax_percentage;
            salary_manager.update_salary_config(level, new_base_salary, new_salary_per_hour, new_salary_per_extra_hour, new_official_working_hours, new_tax_percentage);
        }
        else if (command == ADD_WORKING_HOURS)
        {
            int employee_id, start_hour, finish_hour, day;
            cin >> employee_id >> day >> start_hour >> finish_hour;
            salary_manager.add_working_hours(employee_id, day, start_hour, finish_hour);
        }
        else if (command == DELETE_WORKING_HOURS)
        {
            int employee_id, day;
            cin >> employee_id >> day;
            salary_manager.delete_working_horus(employee_id, day);
        }
        else if (command == UPDATE_TEAM_BONUS)
        {
            int team_id, bonus_percentage;
            cin >> team_id >> bonus_percentage;
            salary_manager.update_team_bonus(team_id, bonus_percentage);
        }
        else if (command == FIND_TEAM_FOR_BONUS)
        {
            salary_manager.find_teams_for_bonus();
        }
        else
        {
            throw_an_error(UNKNOWN_ERROR);
        }
    }
    return 0;
}
