#include <iostream>
#include <vector>
#include <string>
#include<Windows.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
bool CopyToClipboard(const std::string& data) {
    if (::OpenClipboard(NULL)) {
        ::EmptyClipboard();
        HGLOBAL clipbuffer;
        char* buffer;
        clipbuffer = ::GlobalAlloc(GMEM_DDESHARE, data.size() + 1);
        buffer = static_cast<char*>(::GlobalLock(clipbuffer));
        strcpy_s(buffer, data.size() + 1, data.c_str());
        ::GlobalUnlock(clipbuffer);
        ::SetClipboardData(CF_TEXT, clipbuffer);
        ::CloseClipboard();
        return true;
    }
    return false;
}

std::string base64_encode(const std::string& input) {
    BIO* bio, * b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string encoded(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);

    return encoded;
}
std::string base64_decode(const std::string& input) {
    BIO* bio, * b64;
    char* buffer = new char[input.size()];
    memset(buffer, 0, input.size());

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(input.c_str(), input.length());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    int decoded_length = BIO_read(bio, buffer, input.length());

    BIO_free_all(bio);

    std::string decoded(buffer, decoded_length);

    delete[] buffer;

    return decoded;
}
const std::string b64 = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+/=";

const std::vector<char> leftEye = { 'o', '0', 'O', 'Q' };
const std::vector<char> mouth = { 'w', 'v', '.', '_' };
const std::vector<char> rightEye = { 'o', '0', 'O', 'Q' };
const std::string separator = " ";

std::vector<std::string> table;

void makeTable() {
    for (char i : leftEye) {
        for (char j : mouth) {
            for (char k : rightEye) {
                std::string emoji;
                emoji.push_back(i);
                emoji.push_back(j);
                emoji.push_back(k);
                table.push_back(emoji);
            }
        }
    }
}

std::string addCalls(const std::string& t) {
    return t;
}

std::string human2zdjd(const std::string& t) {
    std::string encoded = base64_encode(t);
    size_t len = encoded.length();
    std::vector<std::string> arr;

    for (size_t i = 0; i < len; i++) {
        char c = encoded.at(i);
        size_t n = b64.find(c);
        if (n != std::string::npos) {
            arr.push_back(table[n]);
        }
    }

    std::string data = "";
    for (const std::string& emoji : arr) {
        data += emoji;
        data += separator;
    }

    return addCalls(data);
}

std::string zdjd2human(const std::string& t) {
    std::vector<std::string> arr;
    size_t startPos = 0;
    size_t separatorPos = t.find(separator);

    while (separatorPos != std::string::npos) {
        std::string emoji = t.substr(startPos, separatorPos - startPos);
        arr.push_back(emoji);
        startPos = separatorPos + separator.length();
        separatorPos = t.find(separator, startPos);
    }

    std::string result = "";

    for (const std::string& emoji : arr) {
        auto it = std::find(table.begin(), table.end(), emoji);
        if (it != table.end()) {
            size_t n = std::distance(table.begin(), it);
            if (n < b64.length()) {
                result.push_back(b64.at(n));
            }
        }
    }

    std::string decoded = base64_decode(result);
    return decoded;
}

bool isZdjd(const std::string& t) {
    try {
        zdjd2human(t);
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

int main() {
    makeTable();
    std::cout << "尊嘟假嘟（ZDJD）语言翻译器" << std::endl;
    std::cout << "请选择操作模式：" << std::endl;
    std::cout << "1. 编码成 ZDJD" << std::endl;
    std::cout << "2. 解码成人类语言" << std::endl;
    std::cout << "请输入选项（1 或 2）：";

    int choice;
    std::cin >> choice;
    std::cin.ignore(); // 忽略之前的换行符

    if (choice == 1) {
        std::cout << "请输入要编码的文本：";
        std::string text;
        std::getline(std::cin, text);

        std::string zdjd = human2zdjd(text);
        std::cout << "编码后的 尊嘟假嘟语言（已复制到粘贴板）： " << zdjd << std::endl;
        CopyToClipboard(zdjd);
    }
    else if (choice == 2) {
        std::cout << "请输入要解码的 尊嘟假嘟语言：";
        std::string zdjd;
        std::getline(std::cin, zdjd);
        std::string human = zdjd2human(zdjd);
        std::cout << "解码后的文本（已复制到粘贴板）： " << human << std::endl;
        CopyToClipboard(human);
    }
    else {
        std::cout << "无效的选项！" << std::endl;
    }
    system("pause");
    return 0;
}

