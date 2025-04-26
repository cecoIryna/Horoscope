#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Функція для обробки відповіді від cURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Функція для виконання GET-запиту
string HttpGet(const string& url) {
    CURL* curl = curl_easy_init();
    string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Встановлюємо заголовки
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Error cURL: " << curl_easy_strerror(res) << endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return response;
}

// Функція для отримання гороскопу
void GetHoroscope(const string& sign, const string& day) {
    string url = "https://horoscope-app-api.vercel.app/api/v1/get-horoscope/daily?sign=" + sign + "&day=" + day;
    string response = HttpGet(url);

    try {
        auto j = json::parse(response);
        if (j.contains("data") && j["data"].contains("horoscope_data")) {
            cout << "\nHoroscope for " << sign << " for " << (day == "TODAY" ? "today" : "tomorrow") << ":\n";
            cout << j["data"]["horoscope_data"] << endl;
        }
        else {
            cerr << "Incorrect response structure from API." << endl;
        }
    }
    catch (const json::exception& e) {
        cerr << "JSON parsing error: " << e.what() << endl;
        cerr << "Server response: " << response << endl;
    }
}

int main() {
    curl_global_init(CURL_GLOBAL_ALL);

    string signs[] = { "aries", "taurus", "gemini", "cancer", "leo", "virgo",
                       "libra", "scorpio", "sagittarius", "capricorn", "aquarius", "pisces" };

    cout << "Choose your zodiac sign:\n";
    for (int i = 0; i < 12; ++i) {
        cout << " " << (i + 1) << ". " << signs[i] << endl;
    }
    cout << "\nEnter a number (1-12): ";

    int choice;
    cin >> choice;
    if (choice < 1 || choice > 12) {
        cerr << "Wrong choice.\n";
        return 1;
    }

    string selectedSign = signs[choice - 1];

    GetHoroscope(selectedSign, "TODAY");
    GetHoroscope(selectedSign, "TOMORROW");

    curl_global_cleanup();
    return 0;
}
