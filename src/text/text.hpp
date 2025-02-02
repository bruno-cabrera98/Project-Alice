#pragma once

#include <stdint.h>
#include <variant>
#include <vector>
#include <string>
#include <string_view>
#include "dcon_generated.hpp"
#include "unordered_dense.h"

namespace sys {
	struct state;
}

namespace text {
	enum class text_color : uint8_t {
		black, white, red, green, yellow, unspecified, light_blue, dark_blue
	};
	enum class alignment : uint8_t {
		left, right, center
	};
	enum class variable_type : uint16_t {
		error_no_matching_value, action, active, actor, adj, against, agressor, allowed, amount,
		anyprovince, army, army_name, astate, attacker, attunit, bac, badboy, badword, base,
		base_percentage, bat, bld, bon, bonus, brig, bud, building, buy, cap, capital, cash,
		casus, cat, category, cb_target_name, cb_target_name_adj, chan, chance, change, commander,
		con, construction, continentname, cost, count, countries, country, country1, country2,
		countryadj, countryculture, countryname, country_adj, creator, credits, crisisarea,
		crisisattacker, crisisdefender, crisistaker, crisistaker_adj, crisistarget, crisistarget_adj,
		cul, culture, culture_group_union, curr, current, d, date, date_long_0, date_long_1,
		date_short_0, day, days, defender, defunit, desc, dest, details, direction, dist, effect,
		effects, emilist, employees, employee_max, enemy, engineermaxunits, engineerunits, escorts,
		event, eventdesc, exc, exp, faction, factory, first, focus, folder, vtype_for, fraction,
		vtype_friend, from, fromcountry, fromcountry_adj, fromprovince, fromruler, funds, goal,
		good, goods, gov, government, govt, gp, gp_adj, group, high_tax, hit, home, hull, iamount,
		icountry, ide, ideology, ilocation, immlist, impact, iname, income, indep, inf, infamy,
		input, inv, invention, invested, invested_in_us_message, issue, law, leader, lev, level,
		levels, lim, limit, list, literacy, loc, local, location, lord, lose, low_tax, lvl, m,
		max, maxloan, men, messenger, mil, militancy, min, monarchtitle, money, month, months,
		movement, much, name, nation, nationalvalue, natives, navy, navy_name, need, needed,
		negative, neut, vtype_new, newcountry, nf, now, num, number, numfactories, numspecialfactories,
		odds, old, oldcountry, vtype_operator, opinion, opposing_army, opposing_navy, opressor,
		opt, optimal, option, order, org, organisation, other, otherresult, ourcapital, ourcountry,
		ourcountry_adj, our_lead, our_num, our_res, output, overlord, param, party, passive, pay,
		pen, penalty, per, perc, perc2, percent, percentage, player, policy, pop, poptype,
		popularity, position, positive, power, pres, prescence, prestige, produced, producer,
		prog, progress, proj, prov, provinceculture, provincename, provincereligion,
		provinces, provs, province, rank, rate, rec, recipient, reconmaxunits, reconunits, reform,
		region, rel, relation, req, reqlevel, required, resource, result, rstate, rule, runs, scr,
		sea, second, second_country, self, sell, setting, ships, size, skill, source, speed,
		spheremaster, state, statename, str, strata, strings_list_4, string_0_0, string_0_1,
		string_0_2, string_0_3, string_0_4, string_9_0, sub, table, tag, tag0_0, tag_0_0, tag_0_0_,
		tag_0_0_adj, tag_0_0_upper, tag_0_1, tag_0_1_adj, tag_0_1_upper, tag_0_2, tag_0_2_adj,
		tag_0_3, tag_0_3_adj, tag_1_0, tag_2_0, tag_2_0_upper, tag_3_0, tag_3_0_upper, target,
		targetlist, target_country, tech, temperature, terms, terrain, terrainmod, text, theirlost,
		theirnum, theirship, their_lead, their_num, their_res, them, third, threat, time, title, to,
		tot, totalemi, totalimm, tra, truth, type, unemployed, vtype_union, union_adj, unit, units,
		until, usloss, usnum, val, value, value_int1, value_int_0_0, value_int_0_1, value_int_0_2,
		value_int_0_3, value_int_0_4, verb, versus, war, wargoal, we, what, where, which, who, winner,
		x, y, year, years, chief_of_navy, chief_of_staff, control, head_of_government, owner, playername
	};

	struct line_break {
	};

	struct text_sequence {
		uint16_t starting_component = 0;
		uint16_t component_count = 0;
	};

	using text_component = std::variant<line_break, text_color, variable_type, dcon::text_key>;

	struct vector_backed_hash {
		using is_avalanching = void;
		using is_transparent = void;

		std::vector<char>& text_data;

		vector_backed_hash(std::vector<char>& text_data) : text_data(text_data) { }

		auto operator()(std::string_view sv) const noexcept -> uint64_t {
			return ankerl::unordered_dense::detail::wyhash::hash(sv.data(), sv.size());
		}
		auto operator()(dcon::text_key tag) const noexcept -> uint64_t {
			auto sv = [&]() { 
				if(!tag)
					return std::string_view();
				auto start_position = text_data.data() + tag.index();
				auto data_size = text_data.size();
				auto end_position = start_position;
				for(; end_position < text_data.data() + data_size; ++end_position) {
					if(*end_position == 0)
						break;
				}
				return std::string_view(text_data.data() + tag.index(), size_t(end_position - start_position));
			}();
			return ankerl::unordered_dense::detail::wyhash::hash(sv.data(), sv.size());
		}
	};
	struct vector_backed_eq {
		using is_transparent = void;

		std::vector<char>& text_data;

		vector_backed_eq(std::vector<char>& text_data) : text_data(text_data) {}
		
		bool operator()(dcon::text_key l, dcon::text_key r) const noexcept {
			return l == r;
		}
		bool operator()(dcon::text_key l, std::string_view r) const noexcept {
			auto sv = [&]() {
				if(!l)
					return std::string_view();
				auto start_position = text_data.data() + l.index();
				auto data_size = text_data.size();
				auto end_position = start_position;
				for(; end_position < text_data.data() + data_size; ++end_position) {
					if(*end_position == 0)
						break;
				}
				return std::string_view(text_data.data() + l.index(), size_t(end_position - start_position));
			}();
			return sv == r;
		}
		bool operator()(std::string_view r, dcon::text_key l) const noexcept {
			auto sv = [&]() {
				if(!l)
					return std::string_view();
				auto start_position = text_data.data() + l.index();
				auto data_size = text_data.size();
				auto end_position = start_position;
				for(; end_position < text_data.data() + data_size; ++end_position) {
					if(*end_position == 0)
						break;
				}
				return std::string_view(text_data.data() + l.index(), size_t(end_position - start_position));
			}();
			return sv == r;
		}
		bool operator()(dcon::text_key l, std::string const& r) const noexcept {
			auto sv = [&]() {
				if(!l)
					return std::string_view();
				auto start_position = text_data.data() + l.index();
				auto data_size = text_data.size();
				auto end_position = start_position;
				for(; end_position < text_data.data() + data_size; ++end_position) {
					if(*end_position == 0)
						break;
				}
				return std::string_view(text_data.data() + l.index(), size_t(end_position - start_position));
			}();
			return sv == r;
		}
		bool operator()(std::string const& r, dcon::text_key l) const noexcept {
			auto sv = [&]() {
				if(!l)
					return std::string_view();
				auto start_position = text_data.data() + l.index();
				auto data_size = text_data.size();
				auto end_position = start_position;
				for(; end_position < text_data.data() + data_size; ++end_position) {
					if(*end_position == 0)
						break;
				}
				return std::string_view(text_data.data() + l.index(), size_t(end_position - start_position));
			}();
			return sv == r;
		}
	};

	using substitution = std::variant<std::string_view, dcon::text_key, dcon::province_id, dcon::state_instance_id, dcon::nation_id, int64_t, float, sys::date, std::monostate>;
	using substitution_map = ankerl::unordered_dense::map<uint32_t, substitution>;

	struct text_chunk {
		std::string win1250chars;
		float x = 0; // yes, there is a reason the x offset is a floating point value while the y offset is an integer
		substitution source = std::monostate{};
		int16_t y = 0;
		int16_t width = 0;
		int16_t height = 0;
		text_color color = text_color::black;
	};
	struct layout_parameters {
		int16_t left = 0;
		int16_t top = 0;
		int16_t right = 0;
		int16_t bottom = 0;
		uint16_t font_id = 0;
		int16_t leading = 0;
		alignment align = alignment::left;
		text_color color = text_color::white;
	};
	struct layout {
		std::vector<text_chunk> contents;
		int32_t number_of_lines = 0;
		text_chunk const* get_chunk_from_position(int32_t x, int32_t y) const;
	};

	void create_endless_layout(layout& dest, sys::state const& state, layout_parameters const& params, dcon::text_sequence_id source_text, substitution_map const& mp);

	struct layout_box {
		size_t first_chunk = 0;
		int32_t x_offset = 0;
		int32_t x_size = 0;
		int32_t y_size = 0;

		float x_position = 0;
		int32_t y_position = 0;
		text_color color = text_color::white;
	};
	struct columnar_layout {
		layout& base_layout;
		layout_parameters fixed_parameters;
		int32_t used_height = 0;
		int32_t used_width = 0;
		int32_t y_cursor = 0;
		int32_t current_column = 0;
		int32_t column_width = 0;
	};

	layout_box open_layout_box(columnar_layout& dest, int32_t indent);
	void close_layout_box(columnar_layout& dest, layout_box const& box);
	void add_to_layout_box(columnar_layout& dest, sys::state const& state, layout_box& box, dcon::text_sequence_id source_text, substitution_map const& mp);
	void add_to_layout_box(columnar_layout& dest, sys::state const& state, layout_box& box, std::string_view, text_color color, substitution source = std::monostate{});

	columnar_layout create_columnar_layout(layout& dest, layout_parameters const& params, int32_t column_width);


	void add_to_substitution_map(substitution_map& mp, variable_type key, substitution value);
	
	void consume_csv_file(sys::state& state, uint32_t language, char const* file_content, uint32_t file_size);
	variable_type variable_type_from_name(std::string_view);
	void load_text_data(sys::state& state, uint32_t language);
	char16_t win1250toUTF16(char in);
	std::string produce_simple_string(sys::state const& state, dcon::text_sequence_id id);
	std::string produce_simple_string(sys::state const& state, std::string_view key);
	dcon::text_sequence_id find_or_add_key(sys::state& state, std::string_view key);
	std::string date_to_string(sys::state const& state, sys::date date);

	std::string prettify(int32_t num);
	template<class T>
	std::string get_name_as_string(sys::state const& state, T t);
}

