#ifndef IMPIANTO_H
#define IMPIANTO_H

#include <string>
#include <map>
#include <memory>
#include <random>
#include "../Include/DayTime.h"

// ==================== Classe base ====================
class Impianto {
protected:
    std::string nome;
    bool acceso;
    int consumo;

public:
    Impianto(const std::string& n);
    virtual ~Impianto();

    virtual void aggiorna(const DayTime& da, const DayTime& a);
    virtual void stampa() const;

    void accendi();
    void spegni();
    std::string getNome() const;
};

// ==================== Sottoclassi Impianti ====================
class Tropicale : public Impianto {
public:
    Tropicale(const std::string& n);
    void aggiorna(const DayTime& da, const DayTime& a) override;
};

class Desertica : public Impianto {
public:
    Desertica(const std::string& n);
    void aggiorna(const DayTime& da, const DayTime& a) override;
};

class Alpina : public Impianto {
public:
    Alpina(const std::string& n);
    void aggiorna(const DayTime& da, const DayTime& a) override;
};

class Carnivora : public Impianto {
public:
    Carnivora(const std::string& n);
    void aggiorna(const DayTime& da, const DayTime& a) override;
};

class Mediterranea : public Impianto {
private:
    double temperatura;
    std::default_random_engine gen;
    std::uniform_real_distribution<double> dec{0.01, 0.05};
    std::uniform_real_distribution<double> inc{0.75, 1.0};

public:
    Mediterranea(const std::string& n);
    void aggiorna(const DayTime& da, const DayTime& a) override;
    void stampa() const override;
};

// ==================== Classe Serra ====================
class Serra {
private:
    std::map<std::string, std::unique_ptr<Impianto>> impianti;

public:
    void aggiungi(const std::string& nome, const std::string& tipo);
    void aggiornaTutti(const DayTime& da, const DayTime& a);
    void mostra(const std::string& nome = "") const;
    void comanda(const std::string& nome, bool accendi);
};

#endif // IMPIANTO_H
