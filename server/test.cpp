#include "fuuu.hpp"







int msg_test() {
    Msg msg;
    msg.set_desc("signup");
    auto data = msg.mutable_data(); // 这个才可以读
    auto pair = data->insert(mp("uid", makepbv(10086)));
    if(pair.second){
        int m;
        cout << "插入uid成功\n";
        if (getpbo(pair.first->second, m)) {
            cout << "从pair读取uid成功, uid: " << m << '\n';
            auto iter = data->find("uid");
            if (iter != data->cend()) {
                getpbo(iter->second, m);
                cout << "从iter读取uid成功, uid: " << m << '\n';
            }else
                cout << "从iter读取uid失败\n";
        }else
            cout << "从pair读取uid失败\n";
    }
    else
        cout << "插入uid失败\n";
    (*data).insert(mp("words", makepbv("hello world")));
    string sbuff;
    if(msg.SerializeToString(&sbuff)) {
        cout << "序列化成功，长度为：" <<  sbuff.length() << '\n';
        if (msg.ParseFromString(sbuff)) {
            cout << "解析成功：\n";
            cout << "desc: " << msg.desc() << "\n";
            auto data = msg.data();
            auto iter = data.find("uid");
            if (iter != data.cend()) {
                int m;
                if(getpbo(iter->second, m))
                    cout << "uid: " << m << endl;
                else
                    cout << "uid非法值" << endl;
            }
            else
                cout << "没有uid!" << endl;
        }
        else
            cout << "解析失败" << endl;
    }
    else
        cout << "序列化失败" << endl;
    
    return 0;
}



int main() {
    srand((int)time(0));
    WS ws;
    ws_on(ws);
    ws.run();
}