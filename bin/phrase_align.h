#ifndef _PHRASE_ALIGN_H
#define _PHRASE_ALIGN_H

class Phrase_Align {
public:
    Phrase_Align();
    void load_data();

private:
    TermIdxTrans * _term_idx;

    //no copy allowed
    Phrase_Align(const Phrase_Align&);
    void operator=(const Phrase_Align&);
};

#endif
