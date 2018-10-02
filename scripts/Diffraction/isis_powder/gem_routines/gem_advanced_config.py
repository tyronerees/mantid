from __future__ import (absolute_import, division, print_function)

absorption_correction_params = {
    # These are read directly by the generate absorb corrections functions instead of being parsed.
    # Therefore they cannot be overridden using basic config files or keyword arguments.
    "cylinder_sample_height": 4.0,
    "cylinder_sample_radius": 0.4,
    "cylinder_position": [0., 0., 0.],

    "chemical_formula": "V0.9486 Nb0.0514",  # V 94.86% Nb 5.14%
    "number_density": 0.071
}

gem_adv_config_params = {
    "raw_tof_cropping_values": (500, 20000),
    "spline_coefficient": 30
}

focused_cropping_values = [(550, 19900),  # Bank 1
                           (550, 19900),  # Bank 2
                           (550, 19900),  # Bank 3
                           (550, 19900),  # Bank 4
                           (550, 19480),  # Bank 5
                           (550, 17980)  # Bank 6
                           ]

texture_focused_cropping_values = [(448, 29344), (390, 19225), (390, 18673), (438, 28501), (534, 21483), (528, 21255),
                                   (530, 21509), (531, 21563), (531, 21541), (530, 21423), (528, 21295), (520, 20910),
                                   (531, 19801), (534, 19891), (543, 19973), (550, 20043), (548, 20021), (545, 20011),
                                   (538, 19954), (532, 19537), (536, 19583), (530, 19643), (550, 19182), (553, 19284),
                                   (556, 19431), (557, 19560), (557, 19548), (555, 19507), (554, 19356), (546, 19043),
                                   (549, 19176), (546, 19070), (584, 18519), (583, 18481), (579, 18437), (583, 18556),
                                   (582, 18512), (584, 18596), (583, 18570), (583, 18559), (584, 18574), (584, 18530),
                                   (579, 18404), (581, 18452), (581, 18466), (582, 18461), (591, 18249), (589, 18206),
                                   (587, 18135), (592, 18262), (590, 18216), (592, 18286), (593, 18277), (592, 18267),
                                   (593, 18241), (592, 18210), (588, 18139), (589, 18183), (589, 18195), (589, 18136),
                                   (593, 18196), (591, 18072), (602, 17591), (593, 18198), (592, 18158), (594, 18217),
                                   (593, 18175), (591, 18073), (594, 18206), (594, 18220), (603, 17668), (591, 18124),
                                   (591, 18078), (588, 18046), (577, 17402), (578, 17308), (579, 17414), (579, 17423),
                                   (578, 17427), (579, 17432), (577, 17426), (577, 17433), (578, 17443), (577, 17437),
                                   (577, 17428), (578, 17409), (580, 17353), (576, 17379), (576, 17393), (576, 17371),
                                   (576, 17327), (586, 17108), (582, 17172), (574, 17142), (579, 17293), (579, 17298),
                                   (582, 17381), (580, 17330), (581, 17355), (581, 17358), (580, 17337), (581, 17349),
                                   (581, 17341), (580, 17332), (580, 17276), (580, 17314), (582, 17377), (580, 17320),
                                   (586, 17308), (583, 17244), (583, 17254), (584, 17254), (584, 17291), (583, 17271),
                                   (584, 17246), (582, 17223), (584, 17279), (584, 17247), (582, 17238), (583, 17264),
                                   (583, 17271), (594, 16864), (582, 17243), (582, 17239), (582, 17225), (586, 17135),
                                   (598, 16847), (598, 16847), (598, 16839), (599, 16858), (599, 16863), (599, 16862),
                                   (599, 16862), (599, 16829), (598, 16845), (598, 16841), (599, 16849), (599, 16840),
                                   (599, 16852), (599, 16854), (598, 16864), (598, 16860), (599, 16867), (599, 16861),
                                   (599, 16866), (599, 16860), (599, 16836), (598, 16850), (599, 16871), (598, 16850),
                                   (600, 16813), (600, 16822), (600, 16825), (600, 16808), (600, 16799), (600, 16819),
                                   (600, 16794), (600, 16828), (600, 16822), (600, 16827)]


vanadium_cropping_values = [(510, 19997),  # Bank 1
                            (510, 19997),  # Bank 2
                            (510, 19997),  # Bank 3
                            (510, 19997),  # Bank 4
                            (510, 19500),  # Bank 5
                            (510, 18000)  # Bank 6
                            ]

texture_vanadium_cropping_values = [(75, 34933), (65, 22887), (65, 22230), (73, 33929), (89, 25575), (88, 25304),
                                    (89, 25606), (89, 25671), (89, 25644), (89, 25504), (88, 25351), (87, 24893),
                                    (89, 23573), (89, 23679), (91, 23778), (92, 23861), (92, 23834), (91, 23822),
                                    (90, 23755), (89, 23258), (90, 23313), (89, 23385), (92, 22835), (93, 22958),
                                    (93, 23132), (93, 23286), (93, 23271), (93, 23223), (93, 23043), (91, 22670),
                                    (92, 22828), (91, 22703), (98, 22046), (98, 22001), (97, 21949), (98, 22090),
                                    (97, 22038), (98, 22139), (98, 22107), (98, 22094), (98, 22112), (98, 22060),
                                    (97, 21910), (97, 21967), (97, 21984), (97, 21977), (99, 21725), (99, 21674),
                                    (98, 21589), (99, 21740), (99, 21686), (99, 21769), (99, 21759), (99, 21746),
                                    (99, 21715), (99, 21678), (98, 21594), (99, 21647), (99, 21661), (99, 21591),
                                    (99, 21662), (99, 21515), (101, 20942), (99, 21664), (99, 21617), (99, 21688),
                                    (99, 21637), (99, 21515), (99, 21674), (99, 21691), (101, 21034), (99, 21576),
                                    (99, 21521), (98, 21483), (97, 20717), (97, 20605), (97, 20732), (97, 20741),
                                    (97, 20746), (97, 20752), (97, 20746), (97, 20754), (97, 20766), (97, 20758),
                                    (97, 20747), (97, 20725), (97, 20658), (96, 20690), (96, 20706), (96, 20679),
                                    (96, 20628), (98, 20366), (97, 20443), (96, 20407), (97, 20587), (97, 20593),
                                    (97, 20691), (97, 20632), (97, 20661), (97, 20664), (97, 20640), (97, 20653),
                                    (97, 20644), (97, 20634), (97, 20566), (97, 20611), (97, 20687), (97, 20619),
                                    (98, 20605), (98, 20529), (98, 20540), (98, 20540), (98, 20584), (98, 20560),
                                    (98, 20531), (97, 20504), (98, 20571), (98, 20532), (97, 20522), (98, 20553),
                                    (98, 20561), (99, 20076), (97, 20527), (97, 20522), (97, 20507), (98, 20399),
                                    (100, 20056), (100, 20057), (100, 20046), (100, 20069), (100, 20075), (100, 20073),
                                    (100, 20074), (100, 20035), (100, 20054), (100, 20048), (100, 20059), (100, 20048),
                                    (100, 20062), (100, 20064), (100, 20076), (100, 20071), (100, 20080), (100, 20073),
                                    (100, 20079), (100, 20072), (100, 20043), (100, 20060), (100, 20084), (100, 20059),
                                    (100, 20015), (100, 20026), (100, 20030), (100, 20010), (100, 19999), (100, 20023),
                                    (100, 19993), (100, 20034), (100, 20026), (100, 20033)]

texture_mode_on = {"focused_cropping_values": texture_focused_cropping_values,
                   "vanadium_cropping_values": texture_vanadium_cropping_values,
                   "grouping_file_name": "offsets_xie_test_2.cal",
                   "save_maud": True,
                   "save_maud_calib": True,
                   "save_gda": True}

texture_mode_off ={"focused_cropping_values": focused_cropping_values,
                   "vanadium_cropping_values": vanadium_cropping_values,
                   "grouping_file_name": "GEM_Instrument_grouping.cal",
                   "save_maud": False,
                   "save_maud_calib": False,
                   "save_gda": False}

all_adv_variables = {
    "gsas_calib_filename": "GEM_PF1_PROFILE.IPF",
    "maud_grouping_scheme": [1] * 3 + [2] * 8 + [3] * 20 + [4] * 42 + [5] * 52 + [6] * 35,
    "raw_tof_cropping_values": gem_adv_config_params
}


def get_mode_specific_variables(is_texture_mode, is_save_all):
    texture_mode_dict = {}
    if is_texture_mode:
        texture_mode_dict.update(texture_mode_on)
    else:
        texture_mode_dict.update(texture_mode_off)
    texture_mode_dict.update({"save_all": is_save_all})
    return texture_mode_dict


def get_all_adv_variables():
    return all_adv_variables