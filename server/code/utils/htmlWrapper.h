#ifndef UTILS__HTML_WRAPPER_H
#define UTILS__HTML_WRAPPER_H

#include <string>


class HtmlWrapper {
public:
    // positioning elements
    void begin_paragraph();
    void end_paragraph();

    // list
    void begin_list();
    void end_list();
    void push_list_item(std::string text);

    // text elements
    void push_header(std::string text, int level = 1);
    void push_text(std::string text);

    //
    std::string GenerateHtml();

private:
    std::string _inner_html;
    unsigned _paragraph_level = 0;
    unsigned _list_level = 0;
};


#endif  // UTILS__HTML_WRAPPER_H
