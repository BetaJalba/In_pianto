#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iomanip>
#include <random>

using namespace std;

// =============== Classe DayTime ==================
class DayTime {
private:
    int hour, minute;

public:
    DayTime(int h = 0, int m = 0) : hour(h), minute(m) {}

    DayTime(const string& timeStr) {
        stringstream ss(timeStr);
        char sep;
        ss >> hour >> sep >> minute;
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59)
            throw invalid_argument("Formato orario non valido.");
    }

    void advanceMinutes(int min) {
        minute += min;
        hour += minute / 60;
        minute %= 60;
        hour %= 24;
    }

    int diffMinutes(const DayTime& other) const {
        return (other.hour - hour) * 60 + (other.minute - minute);
    }

    string displayTime() const {
        ostringstream oss;
        oss << setw(2) << setfill('0') << hour << ":"
            << setw(2) << setfill('0') << minute;
        return oss.str();
    }

    bool operator==(const DayTime& other) const {
        return hour == other.hour && minute == other.minute;
    }

    friend ostream& operator<<(ostream& os, const DayTime& dt) {
        os << dt.displayTime();
        return os;
    }
};

DayTime now(0, 0); // Orario simulato globale

void logMessage(const DayTime& time, const string& message, const int& errorLevel = 0) {
    if (errorLevel == 0)
        cout << "[" << time << "] " << message << endl;
    else
        cerr << "[" << time << "] " << message << endl;
}

// =============== Classe base Impianto ================
class Impianto {
protected:
    string nome;
    bool acceso = false;
    DayTime ultimoAvvio;
    int consumo = 0;

public:
    Impianto(const string& n) : nome(n) {}
    virtual ~Impianto() = default;

    virtual void aggiorna(const DayTime& da, const DayTime& a) = 0;
    virtual void stampa() const {
        cout << nome << " | Stato: " << (acceso ? "Acceso" : "Spento")
             << " | Consumo: " << consumo << " L" << endl;
    }

    void accendi() {
        acceso = true;
        logMessage(now, "L’impianto '" + nome + "' si è acceso");
    }

    void spegni() {
        acceso = false;
        logMessage(now, "L’impianto '" + nome + "' si è spento");
    }

    string getNome() const { return nome; }
};

// =============== Tipi di Impianti ===================
class Tropicale : public Impianto {
public:
    Tropicale(const string& n) : Impianto(n) {}

    void aggiorna(const DayTime& da, const DayTime& a) override {
        int durata = a.diffMinutes(da);
        int intervallo = 150; // 2.5h
        for (int t = 0; t < durata; t += intervallo) {
            accendi();
            consumo += 10;
            spegni();
        }
    }
};

class Desertica : public Impianto {
public:
    Desertica(const string& n) : Impianto(n) {}
    void aggiorna(const DayTime&, const DayTime&) override {}
};

class Alpina : public Impianto {
public:
    Alpina(const string& n) : Impianto(n) {}
    void aggiorna(const DayTime& da, const DayTime& a) override {
        int durata = a.diffMinutes(da);
        int intervallo = 60;
        for (int t = 0; t < durata; t += intervallo) {
            accendi();
            consumo += 5;
            spegni();
        }
    }
};

class Carnivora : public Impianto {
public:
    Carnivora(const string& n) : Impianto(n) {}
    void aggiorna(const DayTime& da, const DayTime& a) override {
        int durata = a.diffMinutes(da);
        int intervallo = 90;
        for (int t = 0; t < durata; t += intervallo) {
            accendi();
            consumo += 4;
            spegni();
        }
    }
};

class Mediterranea : public Impianto {
private:
    double temperatura = 28.0;

public:
    Mediterranea(const string& n) : Impianto(n) {}

    void aggiorna(const DayTime& da, const DayTime& a) override {
        int minuti = a.diffMinutes(da);
        default_random_engine gen;
        uniform_real_distribution<double> dec(0.01, 0.05);
        uniform_real_distribution<double> inc(0.75, 1.0);

        for (int i = 0; i < minuti; ++i) {
            if (acceso) {
                temperatura += inc(gen) / 60.0;
                if (temperatura >= 28.0) {
                    temperatura = 28.0;
                    spegni();
                }
            } else {
                temperatura -= dec(gen);
                if (temperatura < 25.0) {
                    accendi();
                    consumo += 6;
                }
            }
        }
    }

    void stampa() const override {
        Impianto::stampa();
        cout << "Temperatura attuale: " << temperatura << " °C" << endl;
    }
};

// =============== Classe Serra ======================
class Serra {
private:
    map<string, unique_ptr<Impianto>> impianti;

public:
    void aggiungi(const string& nome, const string& tipo) {
        if (tipo == "Tropicale") impianti[nome] = make_unique<Tropicale>(nome);
        else if (tipo == "Desertica") impianti[nome] = make_unique<Desertica>(nome);
        else if (tipo == "Alpina") impianti[nome] = make_unique<Alpina>(nome);
        else if (tipo == "Carnivora") impianti[nome] = make_unique<Carnivora>(nome);
        else if (tipo == "Mediterranea") impianti[nome] = make_unique<Mediterranea>(nome);
    }

    void aggiornaTutti(const DayTime& da, const DayTime& a) {
        for (auto& [nome, impianto] : impianti)
            impianto->aggiorna(da, a);
    }

    void mostra(const string& nome = "") const {
        if (nome.empty()) {
            for (const auto& [_, impianto] : impianti)
                impianto->stampa();
        } else {
            if (auto it = impianti.find(nome); it != impianti.end())
                it->second->stampa();
            else
                logMessage(now, "Impianto non trovato", 1);
        }
    }

    void comanda(const string& nome, bool accendi) {
        if (auto it = impianti.find(nome); it != impianti.end()) {
            if (accendi) it->second->accendi();
            else it->second->spegni();
        } else logMessage(now, "Impianto non trovato", 1);
    }
};

// =============== Parser comandi ====================
vector<string> commandParser(const string& command) {
    vector<string> tokens;
    istringstream ss(command);
    string token, deviceName;
    bool isName = false;

    while (ss >> token) {
        if (token == "set" || token == "rm" || token == "show") {
            tokens.push_back(token);
            isName = true;
            continue;
        }

        if (isName && (token == "on" || token == "off" || token.find(':') != string::npos)) {
            if (!deviceName.empty()) tokens.push_back(deviceName);
            deviceName.clear();
            isName = false;
        }

        if (isName) {
            if (!deviceName.empty()) deviceName += " ";
            deviceName += token;
        } else {
            tokens.push_back(token);
        }
    }

    if (!deviceName.empty()) tokens.push_back(deviceName);
    return tokens;
}

// =============== Esecuzione comandi ================
void processCommand(Serra& serra, const string& command) {
    vector<string> tokens = commandParser(command);
    if (tokens.empty()) return;

    const string& action = tokens[0];

    if (action == "set") {
        if (tokens[1] == "time") {
            DayTime nuovoOrario(tokens[2]);
            serra.aggiornaTutti(now, nuovoOrario);
            now = nuovoOrario;
            logMessage(now, "L'orario attuale è " + now.displayTime());
        } else {
            const string& nome = tokens[1];
            if (tokens[2] == "on") serra.comanda(nome, true);
            else if (tokens[2] == "off") serra.comanda(nome, false);
        }
    } else if (action == "show") {
        if (tokens.size() == 1) serra.mostra();
        else serra.mostra(tokens[1]);
    }
}

// =============== Main =============================
int main() {
    Serra serra;
    serra.aggiungi("Ficus", "Tropicale");
    serra.aggiungi("Cactus", "Desertica");
    serra.aggiungi("Rosmarino", "Mediterranea");
    serra.aggiungi("Drosera", "Carnivora");
    serra.aggiungi("Pino", "Alpina");

    string input;
    cout << ">> Sistema Serre Intelligenti (digita 'exit' per uscire)\n";

    while (true) {
        cout << "> ";
        getline(cin, input);
        if (input == "exit") break;

        try {
            processCommand(serra, input);
        } catch (const exception& e) {
            logMessage(now, e.what(), 1);
        }
    }

    return 0;
}
