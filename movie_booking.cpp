/*
 * MOVIE TICKET BOOKING SYSTEM  -  DSAL Mini Project (S.Y. B.Tech IT, PICT)
 *
 * Data structures / algorithms used (ONLY from the lab schedule):
 *   Expt 1  Singly Linked List  : list of Screens (add, display, search)
 *   Expt 2  Doubly Linked List  : list of Shows (time slots) of a screen,
 *                                 Next / Previous navigation, insert, delete
 *   Expt 3  Stack (array)       : "Undo last booking"
 *   Expt 4  Queue / Priority Q  : Waiting list when a show is full
 *                                 (VIP customers get higher priority)
 *   Expt 5  Linear Search       : search booking by name, movie by name
 *           Binary Search       : search movie by ID (movies sorted by ID)
 *   Expt 6  Bubble Sort         : movies by rating
 *           Insertion Sort      : movies by name
 *   Expt 7  Quick Sort          : shows sorted by ticket price
 *   Expt 8  Hash Table (separate chaining)      : bookings, key = booking ID
 *           Hash Table (open addressing, linear): customers, key = phone
 *   Expt 9  Sequential File     : bookings.txt (append, display, search, delete)
 *
 * NOTE: Enter names WITHOUT spaces (e.g. Rahul or Rahul_Patil).
 * Compile : g++ movie_booking.cpp -o movie_booking
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
using namespace std;

#define ROWS 5          // seat rows in every screen
#define COLS 6          // seats per row  (5 x 6 = 30 seats)
#define MAXSEATS 6      // max seats in one booking
#define HASH_SIZE 11
#define STACK_MAX 100
#define NUM_MOVIES 6
#define FILE_NAME "bookings.txt"

// ======================= MOVIES (array, sorted by ID) =======================
struct Movie {
    int id;
    string name;
    float rating;
};

Movie movies[NUM_MOVIES] = {
    {1, "Avengers", 8.4},
    {2, "Inception", 8.8},
    {3, "Jawan", 7.0},
    {4, "KGF2", 8.2},
    {5, "Kantara", 8.3},
    {6, "RRR", 7.9}
};

// Expt 5: Binary Search (movies are already sorted by id)
int binarySearchMovie(int id) {
    int low = 0, high = NUM_MOVIES - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (movies[mid].id == id) return mid;
        else if (movies[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

// Expt 5: Linear Search by movie name
int linearSearchMovie(string name) {
    for (int i = 0; i < NUM_MOVIES; i++)
        if (movies[i].name == name) return i;
    return -1;
}

string movieName(int id) {
    int i = binarySearchMovie(id);
    if (i == -1) return "Unknown";
    return movies[i].name;
}

void printMovies(Movie m[]) {
    cout << "\nID   Name        Rating\n";
    for (int i = 0; i < NUM_MOVIES; i++)
        cout << m[i].id << "    " << m[i].name << "\t" << m[i].rating << endl;
}

// Expt 6: Bubble Sort (rating, highest first)
void bubbleSortByRating(Movie m[]) {
    for (int i = 0; i < NUM_MOVIES - 1; i++)
        for (int j = 0; j < NUM_MOVIES - 1 - i; j++)
            if (m[j].rating < m[j + 1].rating) {
                Movie temp = m[j];
                m[j] = m[j + 1];
                m[j + 1] = temp;
            }
}

// Expt 6: Insertion Sort (name, A to Z)
void insertionSortByName(Movie m[]) {
    for (int i = 1; i < NUM_MOVIES; i++) {
        Movie key = m[i];
        int j = i - 1;
        while (j >= 0 && m[j].name > key.name) {
            m[j + 1] = m[j];
            j--;
        }
        m[j + 1] = key;
    }
}

// ================ SCREENS (SLL) and SHOWS (DLL)  Expt 1 & 2 =================
struct Show {                       // node of Doubly Linked List
    int showId;
    int movieId;
    string time;
    float price;
    int seats[ROWS][COLS];          // 0 = free, 1 = booked
    int booked;
    Show *prev, *next;
};

struct Screen {                     // node of Singly Linked List
    int screenNo;
    string name;
    Show *head;                     // head of the DLL of shows
    Screen *next;
};

Screen *screenHead = NULL;

Screen* addScreen(int no, string name) {            // SLL insertion at end
    Screen *sc = new Screen;
    sc->screenNo = no;
    sc->name = name;
    sc->head = NULL;
    sc->next = NULL;
    if (screenHead == NULL) screenHead = sc;
    else {
        Screen *t = screenHead;
        while (t->next != NULL) t = t->next;
        t->next = sc;
    }
    return sc;
}

Screen* findScreen(int no) {                         // SLL searching
    Screen *t = screenHead;
    while (t != NULL) {
        if (t->screenNo == no) return t;
        t = t->next;
    }
    return NULL;
}

void addShow(Screen *sc, int sid, int mid, string time, float price) {   // DLL insertion at end
    Show *s = new Show;
    s->showId = sid;
    s->movieId = mid;
    s->time = time;
    s->price = price;
    s->booked = 0;
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            s->seats[i][j] = 0;
    s->prev = s->next = NULL;

    if (sc->head == NULL) sc->head = s;
    else {
        Show *t = sc->head;
        while (t->next != NULL) t = t->next;
        t->next = s;
        s->prev = t;
    }
}

Show* findShow(int sid, Screen **scOut = NULL) {     // traverse SLL, then each DLL
    for (Screen *sc = screenHead; sc != NULL; sc = sc->next)
        for (Show *s = sc->head; s != NULL; s = s->next)
            if (s->showId == sid) {
                if (scOut) *scOut = sc;
                return s;
            }
    return NULL;
}

bool deleteShow(int sid) {                           // DLL deletion
    Screen *sc;
    Show *s = findShow(sid, &sc);
    if (s == NULL) return false;
    if (s->booked > 0) {
        cout << "Cannot delete: show has bookings.\n";
        return false;
    }
    if (s->prev) s->prev->next = s->next;
    else sc->head = s->next;
    if (s->next) s->next->prev = s->prev;
    delete s;
    return true;
}

void printShowLine(Show *s) {
    cout << "  Show " << s->showId << " | " << s->time << " | "
         << movieName(s->movieId) << " | Rs." << s->price
         << " | Free seats: " << ROWS * COLS - s->booked << endl;
}

void displayAll() {                                   // traversal
    for (Screen *sc = screenHead; sc != NULL; sc = sc->next) {
        cout << "\n" << sc->name << " (Screen " << sc->screenNo << ")\n";
        for (Show *s = sc->head; s != NULL; s = s->next)
            printShowLine(s);
    }
}

void showSeats(Show *s) {
    cout << "\n      SCREEN THIS WAY\n     ";
    for (int j = 0; j < COLS; j++) cout << j + 1 << " ";
    cout << "\n";
    for (int i = 0; i < ROWS; i++) {
        cout << "R" << i + 1 << "   ";
        for (int j = 0; j < COLS; j++)
            cout << (s->seats[i][j] ? "X " : "O ");
        cout << "\n";
    }
    cout << "(O = free, X = booked)\n";
}

// Expt 7: Quick Sort on array of Show pointers by price
void quickSort(Show *a[], int low, int high) {
    if (low >= high) return;
    float pivot = a[high]->price;
    int i = low - 1;
    for (int j = low; j < high; j++)
        if (a[j]->price <= pivot) {
            i++;
            Show *t = a[i]; a[i] = a[j]; a[j] = t;
        }
    Show *t = a[i + 1]; a[i + 1] = a[high]; a[high] = t;
    int p = i + 1;
    quickSort(a, low, p - 1);
    quickSort(a, p + 1, high);
}

void showsByPrice() {
    Show *arr[50];
    int n = 0;
    for (Screen *sc = screenHead; sc != NULL; sc = sc->next)
        for (Show *s = sc->head; s != NULL && n < 50; s = s->next)
            arr[n++] = s;
    quickSort(arr, 0, n - 1);
    cout << "\nShows sorted by price (low to high):\n";
    for (int i = 0; i < n; i++) printShowLine(arr[i]);
}

// ===================== STACK (array)  Expt 3 =====================
int stk[STACK_MAX];
int top = -1;
bool stackEmpty() { return top == -1; }
bool stackFull()  { return top == STACK_MAX - 1; }
void push(int x)  { if (!stackFull()) stk[++top] = x; }
int  pop()        { return stk[top--]; }
int  peek()       { return stk[top]; }

// ============ BOOKINGS: HASH TABLE, SEPARATE CHAINING  Expt 8 ============
struct Booking {
    int id;
    string name, phone;
    int showId;
    int count;
    int r[MAXSEATS], c[MAXSEATS];
    float amount;
    Booking *next;                  // chain pointer
};

Booking *table[HASH_SIZE];
int nextId = 1001;

int hashFn(int id) { return id % HASH_SIZE; }

void insertBooking(Booking *b) {                    // insert at head of chain
    int h = hashFn(b->id);
    b->next = table[h];
    table[h] = b;
}

Booking* searchBooking(int id) {
    for (Booking *t = table[hashFn(id)]; t != NULL; t = t->next)
        if (t->id == id) return t;
    return NULL;
}

void removeBooking(int id) {
    int h = hashFn(id);
    Booking *t = table[h], *prev = NULL;
    while (t != NULL && t->id != id) { prev = t; t = t->next; }
    if (t == NULL) return;
    if (prev == NULL) table[h] = t->next;
    else prev->next = t->next;
    delete t;
}

void printBooking(Booking *b) {
    cout << "\nBooking ID : " << b->id
         << "\nName       : " << b->name
         << "\nPhone      : " << b->phone
         << "\nShow       : " << b->showId;
    Show *s = findShow(b->showId);
    if (s) cout << " (" << movieName(s->movieId) << ", " << s->time << ")";
    cout << "\nSeats      : ";
    for (int i = 0; i < b->count; i++) cout << "R" << b->r[i] + 1 << "-" << b->c[i] + 1 << " ";
    cout << "\nAmount     : Rs." << b->amount << endl;
}

// Expt 5: Linear Search by name (goes through all chains)
void searchBookingByName(string name) {
    bool found = false;
    for (int i = 0; i < HASH_SIZE; i++)
        for (Booking *t = table[i]; t != NULL; t = t->next)
            if (t->name == name) { printBooking(t); found = true; }
    if (!found) cout << "No booking found for " << name << endl;
}

// ========= CUSTOMERS: HASH TABLE, OPEN ADDRESSING (LINEAR) Expt 8 =========
struct Customer {
    string phone;
    int visits;
    bool used;
};
Customer cust[HASH_SIZE];

int phoneHash(string p) {
    int sum = 0;
    for (size_t i = 0; i < p.length(); i++) sum += p[i];
    return sum % HASH_SIZE;
}

void addVisit(string phone) {
    int h = phoneHash(phone);
    for (int i = 0; i < HASH_SIZE; i++) {
        int pos = (h + i) % HASH_SIZE;                // linear probing
        if (!cust[pos].used) {
            cust[pos].used = true;
            cust[pos].phone = phone;
            cust[pos].visits = 1;
            return;
        }
        if (cust[pos].phone == phone) { cust[pos].visits++; return; }
    }
    // table full: ignore (small demo table)
}

int getVisits(string phone) {
    int h = phoneHash(phone);
    for (int i = 0; i < HASH_SIZE; i++) {
        int pos = (h + i) % HASH_SIZE;
        if (!cust[pos].used) return 0;
        if (cust[pos].phone == phone) return cust[pos].visits;
    }
    return 0;
}

// ================== SEQUENTIAL FILE  Expt 9 ==================
void writeRecord(ostream &out, Booking &b) {
    out << b.id << " " << b.name << " " << b.phone << " " << b.showId << " "
        << b.count << " " << b.amount;
    for (int i = 0; i < b.count; i++) out << " " << b.r[i] << " " << b.c[i];
    out << "\n";
}

bool readRecord(istream &in, Booking &b) {
    if (!(in >> b.id >> b.name >> b.phone >> b.showId >> b.count >> b.amount)) return false;
    for (int i = 0; i < b.count; i++) in >> b.r[i] >> b.c[i];
    return true;
}

void fileAppend(Booking *b) {
    ofstream out(FILE_NAME, ios::app);
    writeRecord(out, *b);
}

void fileDisplay() {
    ifstream in(FILE_NAME);
    Booking b;
    int n = 0;
    cout << "\n--- Records in " << FILE_NAME << " ---\n";
    while (readRecord(in, b)) {
        cout << "ID " << b.id << " | " << b.name << " | " << b.phone
             << " | Show " << b.showId << " | Seats " << b.count
             << " | Rs." << b.amount << endl;
        n++;
    }
    if (n == 0) cout << "(file is empty)\n";
}

void fileSearch(int id) {                            // sequential search
    ifstream in(FILE_NAME);
    Booking b;
    while (readRecord(in, b))
        if (b.id == id) {
            cout << "Found in file: " << b.name << ", Show " << b.showId
                 << ", Rs." << b.amount << endl;
            return;
        }
    cout << "Record not found in file.\n";
}

void fileDelete(int id) {                            // copy all except one to temp file
    ifstream in(FILE_NAME);
    ofstream out("temp.txt");
    Booking b;
    while (readRecord(in, b))
        if (b.id != id) writeRecord(out, b);
    in.close();
    out.close();
    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);
}

void loadFromFile() {                                // rebuild memory at start-up
    ifstream in(FILE_NAME);
    Booking b;
    while (readRecord(in, b)) {
        Booking *nb = new Booking(b);
        insertBooking(nb);
        Show *s = findShow(b.showId);
        if (s) {
            for (int i = 0; i < b.count; i++) s->seats[b.r[i]][b.c[i]] = 1;
            s->booked += b.count;
        }
        addVisit(b.phone);
        if (b.id >= nextId) nextId = b.id + 1;
    }
}

// ============== WAITING LIST: PRIORITY QUEUE (linked)  Expt 4 ==============
struct Wait {
    string name, phone;
    int showId, seats;
    int priority;                    // 1 = VIP (high), 2 = normal
    Wait *next;
};
Wait *waitHead = NULL;

void addWait(string name, string phone, int showId, int seats) {
    Wait *w = new Wait;
    w->name = name; w->phone = phone; w->showId = showId; w->seats = seats;
    w->priority = (getVisits(phone) >= 3) ? 1 : 2;
    w->next = NULL;
    // insert after all nodes with same or higher priority (keeps FIFO inside a priority)
    if (waitHead == NULL || w->priority < waitHead->priority) {
        w->next = waitHead;
        waitHead = w;
    } else {
        Wait *t = waitHead;
        while (t->next != NULL && t->next->priority <= w->priority) t = t->next;
        w->next = t->next;
        t->next = w;
    }
    cout << "Added to waiting list (" << (w->priority == 1 ? "VIP" : "Normal") << ").\n";
}

void displayWait() {
    if (waitHead == NULL) { cout << "\nWaiting list is empty.\n"; return; }
    cout << "\n--- Waiting List (served top to bottom) ---\n";
    for (Wait *t = waitHead; t != NULL; t = t->next)
        cout << (t->priority == 1 ? "[VIP]    " : "[Normal] ") << t->name
             << " | Show " << t->showId << " | Seats " << t->seats << endl;
}

// ========================= BOOKING / CANCEL =========================
void createBooking(Show *s, string name, string phone, int r[], int c[], int n) {
    Booking *b = new Booking;
    b->id = nextId++;
    b->name = name;
    b->phone = phone;
    b->showId = s->showId;
    b->count = n;
    for (int i = 0; i < n; i++) { b->r[i] = r[i]; b->c[i] = c[i]; }
    b->amount = n * s->price;
    if (getVisits(phone) >= 3) {
        b->amount = b->amount * 0.9;
        cout << "Loyalty discount of 10% applied!\n";
    }
    b->next = NULL;

    s->booked += n;
    insertBooking(b);          // hash table
    push(b->id);               // stack (for undo)
    addVisit(phone);           // customer hash table
    fileAppend(b);             // sequential file
    cout << "\n===== TICKET CONFIRMED =====";
    printBooking(b);
}

void bookForShow(Show *s) {
    string name, phone;
    int n;
    cout << "Your name (no spaces): "; cin >> name;
    cout << "Phone number: ";          cin >> phone;
    showSeats(s);
    cout << "How many seats (1-" << MAXSEATS << ")? "; cin >> n;
    if (n < 1 || n > MAXSEATS) { cout << "Invalid number of seats.\n"; return; }

    int freeSeats = ROWS * COLS - s->booked;
    if (n > freeSeats) {
        cout << "Only " << freeSeats << " seats left. Join waiting list? (y/n): ";
        char ch; cin >> ch;
        if (ch == 'y' || ch == 'Y') addWait(name, phone, s->showId, n);
        return;
    }

    int r[MAXSEATS], c[MAXSEATS];
    for (int i = 0; i < n; i++) {
        int row, col;
        while (true) {
            cout << "Seat " << i + 1 << " - enter row and column: ";
            cin >> row >> col;
            if (!cin) return;                        // input ended / invalid: stop booking
            if (row < 1 || row > ROWS || col < 1 || col > COLS)
                cout << "Invalid seat, try again.\n";
            else if (s->seats[row - 1][col - 1] == 1)
                cout << "Seat already booked, try again.\n";
            else break;
        }
        s->seats[row - 1][col - 1] = 1;
        r[i] = row - 1;
        c[i] = col - 1;
    }
    createBooking(s, name, phone, r, c, n);
}

// give seats from the waiting list when space is available
void processWaitlist() {
    Wait *cur = waitHead, *prev = NULL;
    while (cur != NULL) {
        Show *s = findShow(cur->showId);
        if (s != NULL && ROWS * COLS - s->booked >= cur->seats) {
            int r[MAXSEATS], c[MAXSEATS], k = 0;
            for (int i = 0; i < ROWS && k < cur->seats; i++)
                for (int j = 0; j < COLS && k < cur->seats; j++)
                    if (s->seats[i][j] == 0) {
                        s->seats[i][j] = 1;
                        r[k] = i; c[k] = j; k++;
                    }
            cout << "\n>> Seats allotted from waiting list to " << cur->name << "\n";
            createBooking(s, cur->name, cur->phone, r, c, cur->seats);
            Wait *del = cur;
            if (prev == NULL) waitHead = cur->next;
            else prev->next = cur->next;
            cur = cur->next;
            delete del;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

bool cancelBooking(int id) {
    Booking *b = searchBooking(id);
    if (b == NULL) return false;
    Show *s = findShow(b->showId);
    if (s != NULL) {
        for (int i = 0; i < b->count; i++) s->seats[b->r[i]][b->c[i]] = 0;
        s->booked -= b->count;
    }
    cout << "Booking " << id << " cancelled. Refund: Rs." << b->amount << endl;
    removeBooking(id);
    fileDelete(id);
    processWaitlist();
    return true;
}

void undoLastBooking() {
    while (!stackEmpty()) {
        int id = pop();
        if (searchBooking(id) != NULL) {          // skip already-cancelled ids
            cout << "Undoing booking " << id << "...\n";
            cancelBooking(id);
            return;
        }
    }
    cout << "Nothing to undo.\n";
}

// ===================== MENUS =====================
void browseScreen() {                              // DLL: Next / Previous
    int no;
    cout << "Enter screen number: "; cin >> no;
    Screen *sc = findScreen(no);
    if (sc == NULL || sc->head == NULL) { cout << "Screen not found / no shows.\n"; return; }
    Show *cur = sc->head;
    char ch;
    do {
        cout << "\n" << sc->name << " -> current show:\n";
        printShowLine(cur);
        cout << "[n] Next  [p] Previous  [b] Book this show  [q] Back : ";
        cin >> ch;
        if (!cin) break;
        if (ch == 'n') {
            if (cur->next) cur = cur->next; else cout << "This is the last show.\n";
        } else if (ch == 'p') {
            if (cur->prev) cur = cur->prev; else cout << "This is the first show.\n";
        } else if (ch == 'b') bookForShow(cur);
    } while (ch != 'q');
}

void bookTicket() {
    displayAll();
    int sid;
    cout << "\nEnter Show ID to book: "; cin >> sid;
    Show *s = findShow(sid);
    if (s == NULL) { cout << "Invalid show ID.\n"; return; }
    bookForShow(s);
}

void movieMenu() {
    int ch;
    Movie copy[NUM_MOVIES];
    do {
        cout << "\n--- MOVIES ---\n1. List movies\n2. Search by ID (Binary Search)"
                "\n3. Search by name (Linear Search)\n4. Sort by rating (Bubble Sort)"
                "\n5. Sort by name (Insertion Sort)\n6. Shows sorted by price (Quick Sort)"
                "\n0. Back\nChoice: ";
        cin >> ch;
        for (int i = 0; i < NUM_MOVIES; i++) copy[i] = movies[i];   // keep original sorted by ID
        if (ch == 1) printMovies(movies);
        else if (ch == 2) {
            int id; cout << "Movie ID: "; cin >> id;
            int i = binarySearchMovie(id);
            if (i == -1) cout << "Movie not found.\n";
            else cout << "Found: " << movies[i].name << " (rating " << movies[i].rating << ")\n";
        } else if (ch == 3) {
            string nm; cout << "Movie name: "; cin >> nm;
            int i = linearSearchMovie(nm);
            if (i == -1) cout << "Movie not found.\n";
            else cout << "Found: ID " << movies[i].id << ", rating " << movies[i].rating << "\n";
        } else if (ch == 4) { bubbleSortByRating(copy); printMovies(copy); }
        else if (ch == 5) { insertionSortByName(copy); printMovies(copy); }
        else if (ch == 6) showsByPrice();
    } while (ch != 0);
}

void adminMenu() {                                 // SLL / DLL operations
    int ch;
    do {
        cout << "\n--- ADMIN ---\n1. Add screen\n2. Add show to a screen"
                "\n3. Delete a show\n4. Update ticket price of a show\n0. Back\nChoice: ";
        cin >> ch;
        if (ch == 1) {
            int no; string nm;
            cout << "Screen number: "; cin >> no;
            cout << "Screen name (no spaces): "; cin >> nm;
            if (findScreen(no)) cout << "Screen already exists.\n";
            else { addScreen(no, nm); cout << "Screen added.\n"; }
        } else if (ch == 2) {
            int no, sid, mid; string t; float p;
            cout << "Screen number: "; cin >> no;
            Screen *sc = findScreen(no);
            if (!sc) { cout << "Screen not found.\n"; continue; }
            cout << "New show ID: "; cin >> sid;
            cout << "Movie ID: "; cin >> mid;
            cout << "Time (e.g. 16:30): "; cin >> t;
            cout << "Price: "; cin >> p;
            if (findShow(sid)) cout << "Show ID already used.\n";
            else if (binarySearchMovie(mid) == -1) cout << "Invalid movie ID.\n";
            else { addShow(sc, sid, mid, t, p); cout << "Show added.\n"; }
        } else if (ch == 3) {
            int sid; cout << "Show ID to delete: "; cin >> sid;
            if (deleteShow(sid)) cout << "Show deleted.\n";
        } else if (ch == 4) {
            int sid; float p;
            cout << "Show ID: "; cin >> sid;
            Show *s = findShow(sid);
            if (!s) cout << "Show not found.\n";
            else { cout << "New price: "; cin >> p; s->price = p; cout << "Price updated.\n"; }
        }
    } while (ch != 0);
}

void fileMenu() {
    int ch;
    do {
        cout << "\n--- BOOKING FILE (Sequential File) ---\n1. Display all records"
                "\n2. Search record by ID\n3. Delete record by ID\n0. Back\nChoice: ";
        cin >> ch;
        if (ch == 1) fileDisplay();
        else if (ch == 2) { int id; cout << "Booking ID: "; cin >> id; fileSearch(id); }
        else if (ch == 3) {
            int id; cout << "Booking ID: "; cin >> id;
            cout << "Tip: use main-menu Cancel to also free the seats.\n";
            fileDelete(id);
            cout << "Record removed from file (if it existed).\n";
        }
    } while (ch != 0);
}

void seedData() {
    Screen *s1 = addScreen(1, "Screen1-IMAX");
    addShow(s1, 101, 2, "10:00", 200);
    addShow(s1, 102, 1, "14:00", 250);
    addShow(s1, 103, 1, "18:30", 300);

    Screen *s2 = addScreen(2, "Screen2-Gold");
    addShow(s2, 201, 3, "11:00", 150);
    addShow(s2, 202, 4, "15:00", 180);
    addShow(s2, 203, 4, "21:00", 200);

    Screen *s3 = addScreen(3, "Screen3-Classic");
    addShow(s3, 301, 5, "10:30", 160);
    addShow(s3, 302, 6, "14:30", 170);
    addShow(s3, 303, 2, "19:00", 220);
}

int main() {
    for (int i = 0; i < HASH_SIZE; i++) { table[i] = NULL; cust[i].used = false; cust[i].visits = 0; }
    seedData();
    loadFromFile();

    int ch;
    do {
        cout << "\n========== MOVIE TICKET BOOKING SYSTEM ==========\n"
                "1. View all screens and shows\n"
                "2. Browse a screen (Next / Previous show)\n"
                "3. Book tickets\n"
                "4. Cancel booking\n"
                "5. Undo last booking\n"
                "6. Search booking (by ID / by name)\n"
                "7. Movies, searching and sorting\n"
                "8. View waiting list\n"
                "9. Booking file (sequential file)\n"
                "10. Admin (screens and shows)\n"
                "0. Exit\nChoice: ";
        cin >> ch;
        if (ch == 1) displayAll();
        else if (ch == 2) browseScreen();
        else if (ch == 3) bookTicket();
        else if (ch == 4) {
            int id; cout << "Booking ID: "; cin >> id;
            if (!cancelBooking(id)) cout << "Booking not found.\n";
        } else if (ch == 5) undoLastBooking();
        else if (ch == 6) {
            int t; cout << "1. By ID   2. By name : "; cin >> t;
            if (t == 1) {
                int id; cout << "Booking ID: "; cin >> id;
                Booking *b = searchBooking(id);
                if (b) printBooking(b); else cout << "Booking not found.\n";
            } else {
                string nm; cout << "Name: "; cin >> nm;
                searchBookingByName(nm);
            }
        } else if (ch == 7) movieMenu();
        else if (ch == 8) displayWait();
        else if (ch == 9) fileMenu();
        else if (ch == 10) adminMenu();
    } while (ch != 0);

    cout << "Thank you! Bookings saved in " << FILE_NAME << endl;
    return 0;
}
