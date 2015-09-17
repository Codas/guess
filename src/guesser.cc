#include "guess/guesser.h"

#include <cmath>
#include <cinttypes>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "guess/string_util.h"

namespace guess {

guesser::guesser(std::vector<std::string> const& candidates)
    : candidates_(candidates) {
  for (auto& candidate : candidates_) {
    normalize(candidate);
  }
}

std::vector<int> guesser::guess(std::string in, int count) const {
  auto matches = match_trigrams(in);
  matches.resize(std::min(static_cast<std::size_t>(100), matches.size()));

  score_exact_word_matches(in, matches);
  matches.resize(std::min(static_cast<std::size_t>(count), matches.size()));

  std::vector<int> ret(matches.size());
  for (int i = 0; i < matches.size(); ++i) {
    ret[i] = matches[i].index;
  }

  return ret;
}

std::vector<guesser::match> guesser::match_trigrams(std::string& in) const {
  std::vector<match> matches;
  matches.reserve(candidates_.size());
  for (int i = 0; i < candidates_.size(); ++i) {
    matches.emplace_back(i);
  }

  normalize(in);

  char const* input = in.c_str();
  double sqrt_len_vec_input = std::sqrt(in.size() - 2);

  char trigram_input[4] = {0};
  char trigram_candidate[4] = {0};

  for (int i = 0; i < candidates_.size(); ++i) {
    int match_count = 0;
    const auto len_vec_candidate = candidates_[i].length() - 2;

    char const* substr_input = input;
    while (substr_input[2] != '\0') {
      trigram_input[0] = substr_input[0];
      trigram_input[1] = substr_input[1];
      trigram_input[2] = substr_input[2];
      ++substr_input;

      char const* substr_candidate = candidates_[i].c_str();
      while (substr_candidate[2] != '\0') {
        trigram_candidate[0] = substr_candidate[0];
        trigram_candidate[1] = substr_candidate[1];
        trigram_candidate[2] = substr_candidate[2];
        ++substr_candidate;

        if (*(uint32_t*) trigram_input == *(uint32_t*) trigram_candidate) {
          ++match_count;
          break;
        }
      }
    }

    double denominator = sqrt_len_vec_input * std::sqrt(len_vec_candidate);
    matches[i].cos_sim = match_count / denominator;
  }
  std::sort(std::begin(matches), std::end(matches));

  return matches;
}

void guesser::score_exact_word_matches(std::string& in,
                                       std::vector<match>& matches) const {
  for (int i = 0; i < matches.size(); ++i) {
    auto& candidate = candidates_[matches[i].index];
    for_each_token(in, [&](char* input_token) {
      for_each_token(candidate, [&](char* candidate_token) {
        if (strcmp(candidate_token, input_token) == 0) {
          matches[i].cos_sim *= 1.33;
          return true;
        }
        return false;
      });
      return false;
    });
  }
  std::sort(std::begin(matches), std::end(matches));
}

void guesser::normalize(std::string& s) {
  replace_all(s, "è", "e");
  replace_all(s, "é", "e");
  replace_all(s, "Ä", "a" );
  replace_all(s, "ä", "a" );
  replace_all(s, "Ö", "o" );
  replace_all(s, "ö", "o" );
  replace_all(s, "Ü", "u" );
  replace_all(s, "ü", "u" );
  replace_all(s, "ß", "ss");
  replace_all(s, "-", " " );
  replace_all(s, "/", " " );
  replace_all(s, ".", " " );
  replace_all(s, ",", " " );
  replace_all(s, "(", " " );
  replace_all(s, ")", " " );

  for (int i = 0; i  < s.length(); ++i) {
    char c = s[i];
    bool is_number = c >= '0' && c <= '9';
    bool is_lower_case_char = c >= 'a' && c <= 'z';
    bool is_upper_case_char = c >= 'A' && c <= 'Z';
    if (!is_number && !is_lower_case_char && !is_upper_case_char) {
      s[i] = ' ';
    }
  }

  replace_all(s, "  ", " ");

  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

}  // namespace guess
