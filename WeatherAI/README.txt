Projekt: WeatherAI (Pogodynka AI)
Autor: Patryk Nalewalski  Nr albumu 164298

Opis:
Aplikacja C++ (Qt) wykorzystująca duże modele językowe do generowania opisów pogody, ciekawostek o podanym mieście oraz automatycznego tworzenia i wykonywania skryptów Python rysujących wykresy (Prognoza na dziś i Średnie temperatury roczne).
Obsługuje lokalne modele poprzez Ollama (np. Bielik) oraz chmurowe poprzez API Google Gemini.

1. WYMAGANIA
=================
- C++17, kompilator (np. MSVC lub MinGW na Windows / GCC na Linux)
- Środowisko Qt6 (wymagane moduły: Widgets, Network, Test)
- CMake 3.16+
- Python 3.x z zainstalowanymi bibliotekami z pliku requirements.txt
- Narzędzie Ollama (jeśli chcesz uruchamiać modele lokalnie)

2. INSTALACJA ŚRODOWISKA PYTHON
=================
Przed uruchomieniem aplikacji upewnij się, że masz zainstalowane wymagane pakiety:
    pip install -r requirements.txt
Wymagane biblioteki to: matplotlib oraz numpy.
Aplikacja w celu wygenerowania wykresu używa polecenia systemowego "python" uruchamianego asynchronicznie w tle.

3. URUCHOMIENIE MODELI (Bielik lokalnie lub Gemini Online)
=================
a) Ollama (Bielik lokalnie):
   Pobierz program Ollama z https://ollama.com/. Uruchom go i wpisz w terminalu:
   ollama run speakleash/bielik-11b-v3.0-instruct:Q4_K_M
   Serwer uruchomi się lokalnie pod adresem http://localhost:11434.

b) Gemini API (Online):
   Pobierz darmowy klucz z platformy Google AI Studio: https://aistudio.google.com/app/apikey
   Klucz należy dodać jako zmienną środowiskową o nazwie GEMINI_API_KEY.
   (Pamiętaj by zrestartować środowisko Qt Creator po dodaniu nowej zmiennej!).

4. KOMPILACJA I URUCHOMIENIE APLIKACJI (Qt Creator)
=================
1. Otwórz plik CMakeLists.txt w środowisku Qt Creator.
2. Skonfiguruj projekt dla Desktop Qt 6.x.x.
3. Wybierz cel "WeatherAI" (aplikacja główna).
4. Kliknij Run (Ctrl+R).

5. URUCHOMIENIE TESTÓW JEDNOSTKOWYCH
=================
Projekt posiada zestaw testów napisanych przy użyciu QTest, sprawdzający logikę aplikacji.
1. W Qt Creator, w lewym dolnym rogu (wybór celu kompilacji), zmień "WeatherAI" na "WeatherTests".
2. Kliknij Run (Ctrl+R) i zaobserwuj wyniki w konsoli "Application Output".
Alternatywnie z terminala po zbudowaniu:
    ./build/WeatherTests
