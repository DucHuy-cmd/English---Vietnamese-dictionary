#include <iostream>
#include <vector>
#include <string>
struct Question {
    std::string questionText;         
    std::vector<std::string> options;   
    int correctOptionIndex;           
};

std::vector<Question> englishQuestions = {
    {
        "Which word is a SYNONYM for 'lovely'?",
        {"ugly", "charming", "hateful", "boring"},
        1
    },
    {
        "Find the SYNONYM for 'smart':",
        {"slow", "weak", "intelligent", "foolish"},
        2
    },
    {
        "What is a SYNONYM for 'beautiful'?",
        {"gorgeous", "plain", "hideous", "unattractive"},
        0
    },
    {
        "Choose the SYNONYM for 'happy':",
        {"sad", "cheerful", "gloomy", "miserable"},
        1
    },
    {
        "Which word means the same as 'fast'?",
        {"slow", "sluggish", "quick", "lethargic"},
        2
    },
    {
        "Choose the SYNONYM for 'large':",
        {"tiny", "huge", "small", "minuscule"},
        1
    },
    {
        "Which word is a SYNONYM for 'difficult'?",
        {"easy", "simple", "hard", "effortless"},
        2
    },
    {
        "Find the SYNONYM for 'begin':",
        {"stop", "finish", "start", "end"},
        2
    },
    {
        "What is a SYNONYM for 'silent'?",
        {"noisy", "loud", "quiet", "talkative"},
        2
    },
    {
        "Choose the SYNONYM for 'accurate'?",
        {"correct", "wrong", "false", "incorrect"},
        0
    },
    {
        "Which word is a SYNONYM for 'brave'?",
        {"cowardly", "fearful", "courageous", "timid"},
        2
    },
    {
        "Find the SYNONYM for 'angry':",
        {"furious", "calm", "happy", "peaceful"},
        0
    },
    {
        "What is a SYNONYM for 'brief'?",
        {"long", "short", "extended", "prolonged"},
        1
    },
    {
        "Choose the SYNONYM for 'broad':",
        {"narrow", "wide", "thin", "slender"},
        1
    },
    {
        "Which word means the same as 'calm'?",
        {"excited", "peaceful", "stormy", "wild"},
        1
    },
    {
        "Find the SYNONYM for 'crooked':",
        {"straight", "bent", "direct", "even"},
        1
    },
    {
        "What is a SYNONYM for 'dangerous'?",
        {"safe", "risky", "secure", "harmless"},
        1
    },
    {
        "Choose the SYNONYM for 'eager':",
        {"excited", "bored", "indifferent", "apathetic"},
        0
    },
    {
        "Which word is a SYNONYM for 'entire'?",
        {"partial", "incomplete", "whole", "fractional"},
        2
    },
    {
        "Find the SYNONYM for 'famous'?",
        {"unknown", "well-known", "obscure", "hidden"},
        1
    },
    {
        "What is a SYNONYM for 'fragile'?",
        {"strong", "tough", "delicate", "durable"},
        2
    },
    {
        "Choose the SYNONYM for 'generous':",
        {"kind", "greedy", "mean", "selfish"},
        0
    },
    {
        "Which word means the same as 'honest'?",
        {"truthful", "deceitful", "fake", "dishonest"},
        0
    },
    {
        "Find the SYNONYM for 'ignore':",
        {"notice", "overlook", "attend", "regard"},
        1
    },
    {
        "What is a SYNONYM for 'lazy'?",
        {"active", "idle", "busy", "diligent"},
        1
    },
    {
        "What is the ANTONYM of 'happy'?",
        {"sad", "joyful", "cheerful", "excited"},
        0
    },
    {
        "Which word is the ANTONYM of 'dark'?",
        {"black", "dim", "light", "shadowy"},
        2
    },
    {
        "Find the ANTONYM for 'hot':",
        {"warm", "cold", "boiling", "burning"},
        1
    },
    {
        "Choose the ANTONYM for 'rich':",
        {"wealthy", "poor", "affluent", "loaded"},
        1
    },
    {
        "What is the ANTONYM of 'strong'?",
        {"weak", "powerful", "robust", "tough"},
        0
    },
    {
        "Which word is the ANTONYM of 'win'?",
        {"victory", "succeed", "lose", "gain"},
        2
    },
    {
        "Find the ANTONYM for 'cheap':",
        {"inexpensive", "affordable", "expensive", "reasonable"},
        2
    },
    {
        "Choose the ANTONYM for 'clean':",
        {"pure", "dirty", "neat", "spotless"},
        1
    },
    {
        "What is the ANTONYM of 'hard'?",
        {"solid", "tough", "soft", "firm"},
        2
    },
    {
        "Which word is the ANTONYM of 'heavy'?",
        {"light", "weighty", "hefty", "massive"},
        0
    },
    {
        "Find the ANTONYM for 'high':",
        {"tall", "low", "elevated", "soaring"},
        1
    },
    {
        "Choose the ANTONYM for 'loud':",
        {"noisy", "quiet", "deafening", "boisterous"},
        1
    },
    {
        "What is the ANTONYM of 'old'?",
        {"ancient", "young", "aged", "elderly"},
        1
    },
    {
        "Which word is the ANTONYM of 'sharp'?",
        {"blunt", "pointed", "keen", "acute"},
        0
    },
    {
        "Find the ANTONYM for 'smooth':",
        {"flat", "rough", "slick", "polished"},
        1
    },
    {
        "Choose the ANTONYM for 'sweet':",
        {"sour", "sugar", "tasty", "delicious"},
        0
    },
    {
        "What is the ANTONYM of 'thick'?",
        {"thin", "heavy", "dense", "wide"},
        0
    },
    {
        "Which word is the ANTONYM of 'tight'?",
        {"loose", "firm", "secure", "fastened"},
        0
    },
    {
        "Find the ANTONYM for 'true':",
        {"correct", "right", "false", "factual"},
        2
    },
    {
        "Choose the ANTONYM for 'visible':",
        {"clear", "noticeable", "hidden", "apparent"},
        2
    },
    {
        "What is the ANTONYM of 'wide'?",
        {"broad", "narrow", "spacious", "vast"},
        1
    },
    {
        "Which word is the ANTONYM of 'always'?",
        {"forever", "never", "constantly", "usually"},
        1
    },
    {
        "Find the ANTONYM for 'brave':",
        {"fearless", "cowardly", "bold", "heroic"},
        1
    },
    {
        "Choose the ANTONYM for 'create':",
        {"destroy", "build", "make", "produce"},
        0
    },
    {
        "What is the ANTONYM of 'deep'?",
        {"shallow", "bottomless", "profound", "low"},
        0
    }
};
