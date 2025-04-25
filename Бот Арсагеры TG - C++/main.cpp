#include "config.h"



// Функция для записи данных 
size_t curlWriteFunc(char* ptr, size_t size, size_t nmemb, string* userdata) // nmemb - колличество данных (символов?) / ptr - указатель на наши данные
{
    size_t result = size * nmemb;

    if (userdata != NULL)
    {
        userdata->append(ptr, size * nmemb); // к userdata добалвяем данные из ptr размером size*nmemb
        result = size * nmemb;
    }
    return result;
}

string convertText(string text) // Заменяет пробелы на %20 // Заменяет #на %23 // \n - %0A
{
    string new_text;
    unsigned int counter = 0;
    for (auto i : text)
    {
        if (i == ' ')
        {
            new_text.push_back('%');
            new_text.push_back('2');
            new_text.push_back('0');
        }
        else if (i == '#')
        {
            new_text.push_back('%');
            new_text.push_back('2');
            new_text.push_back('3');
        }
        else if (i == '\n')
        {
            new_text.push_back('%');
            new_text.push_back('0');
            new_text.push_back('A');
        }
        else
            new_text.push_back(text[counter]);
        counter++;
    }
    return new_text;
}

string cp1251_to_utf8(const char* str) { // Функция для изменения кодировки
    string res;
    int result_u, result_c;
    result_u = MultiByteToWideChar(1251, 0, str, -1, 0, 0);
    if (!result_u)
    {
        return 0;
    }
    wchar_t* ures = new wchar_t[result_u];
    if (!MultiByteToWideChar(1251, 0, str, -1, ures, result_u)) {
        delete[] ures;
        return 0;
    }
    result_c = WideCharToMultiByte(65001, 0, ures, -1, 0, 0, 0, 0);
    if (!result_c) {
        delete[] ures;
        return 0;
    }
    char* cres = new char[result_c];
    if (!WideCharToMultiByte(65001, 0, ures, -1, cres, result_c, 0, 0)) {
        delete[] cres;
        return 0;
    }
    delete[] ures;
    res.append(cres);
    delete[] cres;
    return res;
}

class requests
{
private:
    string url;
    string fullURL;
    string data;
    string code;

    CURLcode curlResult;
    CURL* curl;
public:
    

    requests(string url, string code)
    {
        this->url = url;
        this->fullURL = this->url;
        this->curl = curl_easy_init(); // Создание дескриптора
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data); // Просим дескриптор ЗАПИСАТЬ_ДАННЫЕ в buff
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunc);
            this->code = code;

            if (code == "POST")
            {
                this->code = code;
                curl_easy_setopt(curl, CURLOPT_POST, 1);
            }
        }
        else
        {
            exit(0);
        }
    }

    void enter_PARAM(string param)
    {
        if (code == "GET")
        {
            if (url != fullURL)
            {
                this->fullURL = this->fullURL + '&' + param;
            }
            else if (!param.empty())
            {
                this->fullURL = this->url + '?' + param;
            }
        }
        else if (code == "POST")
        {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, param.c_str());
        }
    }

    string req_get()
    {
        curl_easy_setopt(curl, CURLOPT_URL, cp1251_to_utf8(fullURL.c_str()).c_str()); // Закидваем в дескриптор URL Для запроса

        this->curlResult = curl_easy_perform(curl); // выполнить запрос
        return data;
    }

    size_t codeResult_get()
    {
        return curlResult;
    }

    ~requests()
    {
        curl_easy_cleanup(this->curl);
    }
};

string reqWorldClock()
{
    cout << "\nConnection - worldclock\n";
    string data_unformatted;
    string data_formatted;
    string date;


    requests req(URL_ClockWorld, "GET");
    data_unformatted = req.req_get(); // "currentDateTime":"2025-04-24T07:59Z"
    cout << "\nDisconnect - worldclock\n";

    if (req.codeResult_get() == CURLE_OK)
    {
        if (data_unformatted.find("\"currentDateTime\":\"") != -1)
        {
            data_formatted = data_unformatted.substr(data_unformatted.find("\"currentDateTime\":\""));
            data_formatted = data_formatted.substr(0, data_formatted.find("Z")+1);

            date = data_formatted.substr(data_formatted.find("\":\"")+3);

            return date;
        }
        else
        {
            cout << "\nError DATA_FIND - worldclock";
            
            return "false";
        }
    }
    else
    {
        cout << "\nError DATA_CODE - worldclock";
        return "false";
    }
}

bool synCheckClock(string time) // Пример входных данных 17:14 // Синхронизация ко времени поста (22 вечера)
{
    cout << "Syncronization!!!!\n";
    while (true)
    {
        if (std::stoi(time.substr(0, time.find(':'))) >= 17 && std::stoi(time.substr(0, time.find(':'))) < 18)// Время публикации постов Первое значение на 1 час меньше
        {
            cout << "WAIT MINETS Syncronization!!!!\n";
            //cout << stoi(time.substr(time.find(':')+1, time.size() - 1));
            //std::this_thread::sleep_for(std::chrono::minutes(60 - stoi(time.substr(time.find(':')+1,time.size()-1))));

            cout << "END Syncronization!!!!\n\n";
            return true;
        }
        else
            cout << "WAIT HOURS Syncronization!!!!\n";
            std::this_thread::sleep_for(std::chrono::minutes(58));
    }
}

string processingTimes(string date, bool flag = true) // На вход получает не преобразованную дату На выход в НУЖНОЕ время выводит дату 
// flag = true - без синхронизации // flag - false - синхронизация
{ // Пример 2025-04-24T07:59Z Время -5 от Пермского
    string data[2];

    data[0] = date.substr(0, date.find('T')); // 2025-04-24
    data[1] = date.substr(date.find('T')+1, 5); // 07:59

    if (data[0][data->size() - 1] == '1' || data[0][data->size() - 1] == '0') // 01 10 11 20 21
    {
        if (data[0][data->size() - 2] == '0') // // 2025-04-01 2025-01-01 2025-10-01
        {
            data[0].pop_back();
            data[0].pop_back();
            data[0].pop_back();
            // 2025-04
            char buff = (data[0][data->size() - 1]) - '0';
            if (buff == 0) // 2025-10
            {
                data[0].pop_back();
                data[0].pop_back();
                data[0].push_back('0');
                data[0].push_back('9');
                data[0].push_back('-');
                data[0].push_back('3');
                data[0].push_back('1');
            }
            else if (buff == 1) //  2025-01
            {
                data[0].push_back('-');
                data[0].push_back('0');
                data[0].push_back('1');
            }
            else // 2025-04
            {
                buff = (buff - 1) + '0';
                data[0].pop_back();
                data[0].push_back(buff);
                data[0].push_back('-');
                data[0].push_back('3');
                data[0].push_back('1');
            }
            
        }
        else // 10 11 20 21
        {
            if (data[0][data->size() - 1] == '1') // 11 21 31
            {
                data[0].pop_back();
                data[0].push_back('0');
            }
            else if (data[0][data->size() - 1] == '0') // 10 20 30
            {
                data[0].pop_back(); // 10 -> 1    20 -> 2
                char buff = (((data[0][data->size() - 1]) - '0') - 1) + '0';
                data[0].pop_back();
                data[0].push_back(buff);
                data[0].push_back('9');

            }
        }
    }
    else
    {
        char buff = (((data[0][data->size() - 1]) - '0') - 1) + '0';
        data[0].pop_back();
        data[0].push_back(buff);
    }


    if (flag == false)
    {
        synCheckClock(data[1]);
    }
    
    return data[0];
}

string reqArsagera(string data) // На вход получает дату На вывод значение брижевых ориентиров
{
    requests reqArsagera(URL_Arsagera, "GET");
    reqArsagera.enter_PARAM("date="+data);

    string value = reqArsagera.req_get();
    if (value.find("\"nav_per_share\":") != -1)
    {
        value = value.substr(value.find("\"nav_per_share\":")); // "nav_per_share":15053.77,"total_net_assets":2570159190.29}]}
        value = value.substr(0, value.find(",\"") - 1); // "nav_per_share":15053.77
        value = value.substr(value.find("\":") + 2); // 15053.77

        return value;
    }
    return "0";
}

string convertUrlForPostReq(string text, bool flag = false) // Конвертирует входящие элементы в ссылку
{ // Флаг отвечает за 1 элемент или нет
    if (flag != true)
    {
        text = '&' + text;
    }
    return text;
}

void messageToTelegram(string text)
{
    text = convertText(text);
    string ConvertURL;
    ConvertURL = convertUrlForPostReq("chat_id=" + chat_id, true);
    ConvertURL += convertUrlForPostReq("text=" + text);
    ConvertURL += convertUrlForPostReq("parse_mode=HTML");

    requests reqTelegram(URL_TelegramBOT + bot_token + "/sendMessage", "GET");
    reqTelegram.enter_PARAM(ConvertURL);

    string data = reqTelegram.req_get();
    cout  << data << endl;
}


int main(int argc, char* argv[])

{
    setlocale(LC_ALL, "ru_RU.UTF-8");
    cout << "Запуск программы \n";

    int preValueMetrik = 0;
    float diffMetrik_f = 0;
    string diffMetrik_s;
    int valueMetrik;
    string smile;
    string unformatedDate;
    int buffValue;
    string date;

    //string date = processingTimes("2025-10-01T07:59Z", true); // test

    while (true) // Рабочий цикл 
    {
        unformatedDate = reqWorldClock();
        while (unformatedDate == "false")
        {
            std::this_thread::sleep_for(std::chrono::seconds(120));
            unformatedDate = reqWorldClock();
        }
        date = processingTimes(unformatedDate, false);

        buffValue = stoi(reqArsagera(date));

        if (buffValue != 0)
        {
            valueMetrik = buffValue;


            if (valueMetrik != 0 and preValueMetrik != 0)
            {
                if (valueMetrik / preValueMetrik >= 1)
                {
                    diffMetrik_f = round(((float)valueMetrik / preValueMetrik - 1) * 1000) / 10;
                    diffMetrik_s = std::to_string(diffMetrik_f);
                    diffMetrik_s.resize(3);
                    smile = "%F0%9F%93%88";
                }
                else
                {
                    diffMetrik_f = round(((float)valueMetrik / preValueMetrik - 1) * 1000) / 10;
                    diffMetrik_s = std::to_string(diffMetrik_f);
                    diffMetrik_s.resize(3);
                    smile = "%F0%9F%93%89";
                }

                preValueMetrik = valueMetrik;
                messageToTelegram("%F0%9F%92%B0Биржевые ориентиры <b>Арсагера ФА</b>%F0%9F%92%B0 \n\nСтоимость пая на дату <b>" + date + "</b> — <b><u>" + std::to_string(valueMetrik) + "</u></b> рублей \n\nЦена за пай изменилась на <b>" + diffMetrik_s + "%" + smile + "</b> \n\n#Арсагера_ФА");
            }
            else
            {
                // %F0%9F%92%B0 - 💰
                messageToTelegram("%F0%9F%92%B0Биржевые ориентиры <b>Арсагера ФА</b>%F0%9F%92%B0 \n\nСтоимость пая на дату <b>" + date + "</b> — <b><u>" + std::to_string(valueMetrik) + "</u></b> рублей \n\n#Арсагера_ФА");
                preValueMetrik = valueMetrik;
            }

            
        }
        else
            cout << "/nМетрика не найдена" << endl;

        std::this_thread::sleep_for(std::chrono::hours(20));

        

    }

    cout << "\nКонец всему \n";
    return 0;

}