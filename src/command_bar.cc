#include "command_bar.h"
#include <thread>
#include <sstream>
#include "fund.h"
#include <array>

namespace BigMoney {
CommandBar::CommandBar(int x, int y, int startx, int starty) 
    :Window(x, y, startx, starty){
}

CommandBar::~CommandBar() {
    if (listen_thread_ && listen_thread_->joinable()) {
        listen_thread_->join();
    }
}
void CommandBar::GetCommand() {
    is_listen_ = true;
    listen_thread_ = new std::thread([this]{
        std::array<char, 256> input_buf;
        while(is_listen_) {
            wclear(win_);
            wgetnstr(win_, input_buf.data(), input_buf.size());
            ParseCommand(input_buf.data());
        }
    });
}

void CommandBar::ParseCommand(const std::string &cmd) {
    if (cmd.empty()) {
        return;
    }
    std::istringstream is(cmd);
    std::string action;
    is >> action;
    if (action == "add" || action == "update") {
        std::string fund_id;
        float fund_share = 0;
        is >> fund_id >> fund_share;
        if (IsNumber(fund_id)) {
            Fund *fund = new Fund();
            fund->fund_code = fund_id;
            fund->share = fund_share;
            Msg msg = {kUpdateFund, fund};
            PostMsg(msg);
        }
    } else if (action == "delete") {
        std::string *fund_id = new std::string();
        is >> *fund_id;
        if (IsNumber(fund_id->c_str())) {
            Msg msg{kDeleteFund, fund_id};
            PostMsg(msg);
        }
    } else if (action == "info") {
    } else if (action == "quit") {
        Msg msg = {kQuit, nullptr};
        is_listen_ = false;
        PostMsg(msg);
    } else {
        UPDATE_STATUS("无效命令: %s", cmd.c_str());
    }
}

} // namespace BigMoney