#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iomanip>
#include <random>
#include <stdexcept>

#include "../Include/impianto.h"
#include "../Include/DayTime.h"

using namespace std;




DayTime now(0, 0); // Orario simulato globale

// =============== Classe base Impianto ================
Impianto::Impianto(const string &n) : nome(n) {

}
Impianto::~Impianto() = default;
void Impianto::aggiorna(const __resharper_unknown_type &da, const __resharper_unknown_type &a);
void Impianto::stampa() const {
    cout << nome << " | Stato: " << (acceso ? "Acceso" : "Spento")
        << " | Consumo: " << consumo << " L" << endl;
}
void Impianto::accendi() {
    if (!acceso) {
        acceso = true;
        now.logMessage(now, "L’impianto '" + nome + "' si è acceso");
    }
}
void Impianto::spegni() {
    if (acceso) {
        acceso = false;
        now.logMessage(now, "L’impianto '" + nome + "' si è spento");
    }
}
string Impianto::getNome() const {
    return nome;
}

// =============== Tipi di Impianti ===================

    Tropicale::Tropicale(const string& n) : Impianto(n) {}

    void Tropicale::aggiorna(const DayTime& da, const DayTime& a)  {
        int durata = a.diffMinutes(da);
        if (durata <= 0) return;
        int intervallo = 150; // 2.5h in minuti
        for (int t = 0; t < durata; t += intervallo) {
            accendi();
            consumo += 10;
            spegni();
        }
    }



   Desertica::Desertica(const string& n) : Impianto(n) {}
    void Desertica::aggiorna(const DayTime&, const DayTime&)  {
        // Nessuna azione per Desertica
    }



    Alpina::Alpina(const string &n) : Impianto(n) {

    }
   void Alpina::aggiorna(const DayTime& da, const DayTime& a)  {
        int durata = a.diffMinutes(da);
        if (durata <= 0) return;
        int intervallo = 60; // 1h
        for (int t = 0; t < durata; t += intervallo) {
            accendi();
            consumo += 5;
            spegni();
        }
    }



   Carnivora::Carnivora(const string& n) : Impianto(n) {}
    void Carnivora::aggiorna(const DayTime& da, const DayTime& a)  {
        int durata = a.diffMinutes(da);
        if (durata <= 0) return;
        int intervallo = 90; // 1.5h
        for (int t = 0; t < durata; t += intervallo) {
            accendi();
            consumo += 4;
            spegni();
        }
    }



  Mediterranea::Mediterranea(const string& n) : Impianto(n), temperatura(28.0),
    dec(0.01, 0.05), inc(0.75, 1.0) {}

void Mediterranea::aggiorna(const DayTime& da, const DayTime& a) {
    int minuti = a.diffMinutes(da);
    if (minuti <= 0) return;

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

void Mediterranea::stampa() const {
    Impianto::stampa();
    cout << "Temperatura attuale: " << temperatura << " °C" << endl;
}

// =============== Classe Serra ======================
void Serra::aggiungi(const std::string& nome, const std::string& tipo) {
    if (tipo == "Tropicale") impianti[nome] = std::make_unique<Tropicale>(nome);
    else if (tipo == "Desertica") impianti[nome] = std::make_unique<Desertica>(nome);
    else if (tipo == "Alpina") impianti[nome] = std::make_unique<Alpina>(nome);
    else if (tipo == "Carnivora") impianti[nome] = std::make_unique<Carnivora>(nome);
    else if (tipo == "Mediterranea") impianti[nome] = std::make_unique<Mediterranea>(nome);
    else now.logMessage(now, "Tipo di impianto sconosciuto: " + tipo, 1);
}

void Serra::aggiornaTutti(const DayTime& da, const DayTime& a) {
    for (auto& [nome, impianto] : impianti)
        impianto->aggiorna(da, a);
}

void Serra::mostra(const std::string& nome) const {
    if (nome.empty()) {
        for (const auto& [_, impianto] : impianti)
            impianto->stampa();
    } else {
        if (auto it = impianti.find(nome); it != impianti.end())
            it->second->stampa();
        else
            now.logMessage(now, "Impianto non trovato: " + nome, 1);
    }
}

void Serra::comanda(const std::string& nome, bool accendi) {
    if (auto it = impianti.find(nome); it != impianti.end()) {
        if (accendi) it->second->accendi();
        else it->second->spegni();
    } else now.logMessage(now, "Impianto non trovato: " + nome, 1);
}


// =============== Parser comandi ====================
vector<string> commandParser(const string& command) {
    vector<string> tokens;
    istringstream ss(command);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// =============== Esecuzione comandi ================
void processCommand(Serra& serra, const string& command) {
    vector<string> tokens = commandParser(command);
    if (tokens.empty()) return;

    const string& action = tokens[0];

    if (action == "set") {
        if (tokens.size() < 3) {
            now.logMessage(now, "Comando 'set' incompleto", 1);
            return;
        }
        if (tokens[1] == "time") {
            try {
                DayTime nuovoOrario(tokens[2]);
                serra.aggiornaTutti(now, nuovoOrario);
                now = nuovoOrario;
                now.logMessage(now, "L'orario attuale è " + now.displayTime());
            } catch (const exception& e) {
                now.logMessage(now, e.what(), 1);
            }
        } else {
            // set <nome> on/off
            if (tokens.size() < 3) {
                now.logMessage(now, "Comando 'set' incompleto", 1);
                return;
            }
            const string& nome = tokens[1];
            if (tokens[2] == "on") serra.comanda(nome, true);
            else if (tokens[2] == "off") serra.comanda(nome, false);
            else now.logMessage(now, "Comando 'set' non riconosciuto", 1);
        }
    } else if (action == "show") {
        if (tokens.size() == 1) serra.mostra();
        else serra.mostra(tokens[1]);
    } else {
        now.logMessage(now, "Comando sconosciuto: " + action, 1);
    }
}

