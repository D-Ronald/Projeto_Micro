# 💧 Sistema de Contagem de Gotas com Microcontrolador 

Projeto final da disciplina de **Microcontroladores e Microprocessadores**, que consiste no desenvolvimento de um sistema embarcado para contagem precisa de gotas utilizando sensor infravermelho, interrupção externa e exibição de dados em LCD 16x2 via I2C.

## 🔍 Visão Geral

Este sistema tem como objetivo monitorar a passagem de gotas em tempo real, útil em aplicações laboratoriais, biomédicas, químicas e médicas, como controle de infusões intravenosas, dosagem de reagentes e experimentos automatizados.

A contagem é feita com alta precisão através de uma interrupção externa (INT0), e o sistema permite definir uma **meta de gotas**, que é monitorada continuamente. Um buzzer fornece feedback sonoro em diferentes eventos.

## 🎯 Funcionalidades

- Contagem de gotas via sensor infravermelho com interrupção INT0.
- Exibição da meta e da contagem atual em LCD 16x2 (I2C).
- 4 botões de controle:
  - ➕ Aumentar meta
  - ➖ Diminuir meta
  - 🔄 Resetar contagem
  - ▶️ Iniciar/parar contagem
- Feedback sonoro com padrões distintos (buzzer).
- Interface simples, intuitiva e responsiva.

## 🛠️ Tecnologias e Componentes

- **Microcontrolador:** ATmega328P (ou equivalente AVR)
- **Sensor:** Infravermelho (detecção por feixe interrompido)
- **LCD:** 16x2 com módulo I2C (PCF8574)
- **Buzzer:** Piezoelétrico
- **Push-buttons:** 4, com técnica de pull-down
- **Linguagem:** C (AVR-GCC)
- **Plataforma de testes:** Protoboard + Fonte 5V

## 🔌 Diagrama de Ligações (Resumo)

| Pino AVR | Componente         | Função                  |
|----------|--------------------|--------------------------|
| PD2      | Sensor IR          | Interrupção INT0         |
| PB1–PB4  | Botões             | Esquerdo, Meio, Direito, Reset |
| PB0      | Buzzer             | Feedback sonoro          |
| SDA/SCL  | Módulo I2C (LCD)   | Comunicação com LCD       |

## 📦 Como Usar

1. Faça o clone do repositório:
   ```bash
   git clone https://github.com/D-Ronald/Projeto_Micro.git
