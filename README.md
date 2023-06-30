### Lebada Daria-Cristiana, 323CA
## Tema2 PCom - Aplicatie client-server TCP si UDP

# Cum se utilizeaza:
  - se ruleaza ./server 12345
  - se ruleaza clienti tcp: ex ./subscriber C1 127.0.0.1 12345
  - dupa ce se aboneaza clientii, se trimit mesaje de la clientii udp:
  python3 udp_client.py --source-port 1234 --input_file three_topics_payloads.json --mode random --delay 2000 127.0.0.1 12345

# Functionare aplicatie
Pentru implementarea celor doua programe m-am folosit de laboratoarele 5 (pentru socket UDP)
si 7 (pentru socket TCP, multiplexare si DIE() - helpers.h).

In server deschid un socket TCP si unul UDP pe care ii adaug in structura pfds (alaturi de
file descriptorul pentru citirea datelor de la stdin) care se va ocupa cu monitorizarea evenimentelor.
Apoi intru intr-un loop de server unde incep sa prelucrez informatiile primite de la clienti. 
Prima data verific daca se doreste introducerea datelor de la stdin (unde pentru server exista o singura
optiune valida - "exit"). In acest caz trebuie sa deconectez fiecare client conectat (trimit un mesaj de exit
catre fiecare client conectat), sa ii elimin din baza de date si sa inchid socketii.
Daca nu se doreste inchiderea serverului, atunci am 3 cazuri posibile:
- Primesc informatii de la clientul TCP: aici voi primi ID-ul clientului si trebuie sa verific daca
e client nou sau se reconecteaza un client mai vechi, caz in care ii voi afisa mesajele primite cat a
fost deconectat.
- Primesc informatii de la clientul UDP: aici primesc informatii despre topicuri. Preiau informatia 
primita si o transfer intr-o structura de tip udp_message_t si imi construiesc mesajul care va fi
trimis la subscriberi in functie de tipul mesajului si abonarile clientilor. Daca subscriberul nu este
conectat, dar are un topic cu sf 1, atunci retinem mesajele primite intr-o coada.
- Primesc mesaj de la un client deja conectat: daca nu primesc niciun mesaj, atunci clientul s-a
deconectat (inchid conexiunea si il elimin din pfds), altfel primesc mesajul si realizez operatia ceruta de
subscribe, respectiv unsubscribe.

Subscriberul este foarte asemanator cu serverul, doar ca aici am doar un socket tcp si prelucrez
doar informatiile primite de la tastatura sau de la server.

# Protocol TCP implementat
Pentru implementarea unui protocol cat mai eficient, am avut in vedere urmatoarele aspecte:
- Am dezactivat algoritmul lui Nagle pentru a evita existenta unui delay la trimiterea mesajelor.
- Am avut grija sa nu trimit mai multe date decat este necesar, astfel ca inainte de a trimitere
mesajele propriu-zise, am grija sa trimit dimensiunea exacta a datelor, astfel nu trimit date inutile
si evit sa primesc date corupte. La fel si la primire, unde stiu exact dimensiunea datelor receptionate.
Dimensiunea datelor o trimit ca un uint32_t (orice dimensiune trimisa se va incadra in 4 octeti),
pentru a avea siguranta ca voi avea aceleasi dimensiuni pe orice platforma. De asemenea, trimit
structuri ca __attribute__ ((__packed__)) pentru a ma asigura ca nu trimit date inutile.
- Operatiile sunt realizate eficient, deoarece clientii primesc mesaje doar de la topicurile la
care sunt abonati.

# Observatii
A fost o tema interesanta, greuta la inceput, mai usoara dupa recitirea de cateva ori a laboratoarelor.
Chiar am reusit sa inteleg cum se lucreaza cu socketi si mi s-a parut chiar interesanta tema per total.
