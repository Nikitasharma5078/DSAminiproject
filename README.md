# Movie Ticket Booking System

A menu-driven C++ console application for managing movie shows, seat reservations, cancellations, waiting lists, and booking records. It was created as a Data Structures and Algorithms mini project.

## Requirements

- A C++ compiler with C++11 support (such as `g++`)

## Build and run

```bash
g++ -std=c++11 movie_booking.cpp -o movie_booking
./movie_booking
```

On Windows, run the executable with:

```bat
movie_booking.exe
```

## Features

- View movie screens, shows, ticket prices, and seat availability
- Book up to six seats per transaction and cancel bookings
- Undo the most recent booking
- Maintain a priority waiting list for full shows; VIP customers are served first
- Search movies by ID or name and search bookings by ID or customer name
- Sort movies by rating or name, and shows by ticket price
- Admin functions to add screens and shows, delete empty shows, and update prices
- Persist booking records in `bookings.txt`

## Data structures and algorithms

| Concept | Use in the project |
| --- | --- |
| Singly linked list | Screens |
| Doubly linked list | Shows for each screen and next/previous navigation |
| Stack | Undo last booking |
| Priority queue | VIP waiting list |
| Hash tables | Booking lookup and customer records |
| Linear and binary search | Movie and booking searches |
| Bubble, insertion, and quick sort | Sorting movies and shows |
| Sequential file | Persistent booking records |

## Notes

- Enter names without spaces (for example, `Rahul` or `Rahul_Patil`).
- `bookings.txt` is generated in the directory where the program is run and is loaded on the next launch.
