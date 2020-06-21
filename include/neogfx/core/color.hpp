// color.hpp
/*
  neogfx C++ App/Game Engine
  Copyright (c) 2015, 2020 Leigh Johnston.  All Rights Reserved.
  
  This program is free software: you can redistribute it and / or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <neogfx/neogfx.hpp>
#include <type_traits>
#include <neolib/core/variant.hpp>
#include <neogfx/core/geometrical.hpp>
#include <neogfx/gui/layout/i_geometry.hpp>
#include <neogfx/core/hsl_color.hpp>
#include <neogfx/core/hsv_color.hpp>

namespace neogfx
{
    enum class color_space : uint32_t
    {
        LinearRGB,
        sRGB
    };

    template <typename Color, typename Derived>
    struct color_return_type_cracker
    {
        typedef Derived return_type;
    };

    template <typename Color>
    struct color_return_type_cracker<Color, void>
    {
        typedef Color return_type;
    };

    template <color_space ColorSpace, typename BaseComponent, typename ViewComponent = BaseComponent, typename Derived = void>
    class basic_color : public basic_vector<BaseComponent, 4>
    {
        typedef basic_color<ColorSpace, BaseComponent, ViewComponent, Derived> self_type;
        typedef basic_vector<BaseComponent, 4> base_type;
        typedef Derived derived_type;
    public:
        typedef BaseComponent base_component;
        typedef ViewComponent view_component;
        typedef typename color_return_type_cracker<self_type, derived_type>::return_type return_type;
    public:
        using base_type::base_type;
        basic_color(const basic_vector<BaseComponent, 3>& aNoAlpha) :
            base_type{ aNoAlpha.x, aNoAlpha.y, aNoAlpha.z, static_cast<BaseComponent>(1.0) } {}
    public:
        self_type& operator=(const self_type& aOther)
        {
            base_type::operator=(aOther);
            return *this;
        }
    public:
        template <typename To, typename From>
        static To convert(From aComponent)
        {
            if constexpr (std::is_same_v<From, To>)
                return aComponent;
            else if constexpr (std::is_integral_v<To> && std::is_floating_point_v<From>)
                return static_cast<To>(aComponent * std::numeric_limits<To>::max());
            else if constexpr (std::is_floating_point_v<To> && std::is_integral_v<From>)
            {
                if constexpr (!std::is_integral_v<From> || !std::is_integral_v<view_component>)
                    return static_cast<To>(aComponent) / std::numeric_limits<From>::max();
                else
                    return static_cast<To>(aComponent) / std::numeric_limits<view_component>::max();
            }
            else
                return static_cast<To>(aComponent);
        }
        static view_component base_to_view(base_component aComponent)
        {
            return convert<view_component>(aComponent);
        }
        view_component red() const
        {
            return base_to_view(base_type::operator[](0));
        }
        view_component green() const
        {
            return base_to_view(base_type::operator[](1));
        }
        view_component blue() const
        {
            return base_to_view(base_type::operator[](2));
        }
        view_component alpha() const
        {
            return base_to_view(base_type::operator[](3));
        }
        template <typename T>
        T red() const
        {
            return convert<T>(base_type::operator[](0));
        }
        template <typename T>
        T green() const
        {
            return convert<T>(base_type::operator[](1));
        }
        template <typename T>
        T blue() const
        {
            return convert<T>(base_type::operator[](2));
        }
        template <typename T>
        T alpha() const
        {
            return convert<T>(base_type::operator[](3));
        }
        return_type& set_red(view_component aRed)
        {
            base_type::operator[](0) = convert<base_component>(aRed);
            return static_cast<return_type&>(*this);
        }
        return_type& set_green(view_component aGreen)
        {
            base_type::operator[](1) = convert<base_component>(aGreen);
            return static_cast<return_type&>(*this);
        }
        return_type& set_blue(view_component aBlue)
        {
            base_type::operator[](2) = convert<base_component>(aBlue);
            return static_cast<return_type&>(*this);
        }
        return_type& set_alpha(view_component aAlpha)
        {
            base_type::operator[](3) = convert<base_component>(aAlpha);
            return static_cast<return_type&>(*this);
        }
        template <typename T>
        return_type& set_red(T aRed)
        {
            base_type::operator[](0) = convert<base_component>(aRed);
            return static_cast<return_type&>(*this);
        }
        template <typename T>
        return_type& set_green(T aGreen)
        {
            base_type::operator[](1) = convert<base_component>(aGreen);
            return static_cast<return_type&>(*this);
        }
        template <typename T>
        return_type& set_blue(T aBlue)
        {
            base_type::operator[](2) = convert<base_component>(aBlue);
            return static_cast<return_type&>(*this);
        }
        template <typename T>
        return_type& set_alpha(T aAlpha)
        {
            base_type::operator[](3) = convert<base_component>(aAlpha);
            return static_cast<return_type&>(*this);
        }
        return_type with_red(view_component aRed) const
        {
            return return_type{ *this }.set_red(aRed);
        }
        return_type with_green(view_component aGreen) const
        {
            return return_type{ *this }.set_green(aGreen);
        }
        return_type with_blue(view_component aBlue) const
        {
            return return_type{ *this }.set_blue(aBlue);
        }
        return_type with_alpha(view_component aAlpha) const
        {
            return return_type{ *this }.set_alpha(aAlpha);
        }
        template <typename T>
        return_type with_red(T aRed) const
        {
            return return_type{ *this }.set_red(aRed);
        }
        template <typename T>
        return_type with_green(T aGreen) const
        {
            return return_type{ *this }.set_green(aGreen);
        }
        template <typename T>
        return_type with_blue(T aBlue) const
        {
            return return_type{ *this }.set_blue(aBlue);
        }
        template <typename T>
        return_type with_alpha(T aAlpha) const
        {
            return return_type{ *this }.set_alpha(aAlpha);
        }
        return_type with_combined_alpha(view_component aAlpha) const
        {
            return return_type{ *this }.with_alpha(convert<scalar>(alpha()) * convert<scalar>(aAlpha));
        }
        template <typename T>
        return_type with_combined_alpha(T aAlpha) const
        {
            return return_type{ *this }.with_alpha(convert<scalar>(alpha()) * convert<scalar>(aAlpha));
        }
    };

    using linear_color = basic_color<color_space::LinearRGB, scalar>;

    class sRGB_color : public basic_color<color_space::sRGB, scalar, uint8_t, sRGB_color>
    {
        typedef basic_color<color_space::sRGB, scalar, uint8_t, sRGB_color> base_type;
        // types
    public:
        typedef view_component component;
        typedef uint32_t argb;
        // constants
    public:
        static const sRGB_color AliceBlue;
        static const sRGB_color AntiqueWhite;
        static const sRGB_color AntiqueWhite1;
        static const sRGB_color AntiqueWhite2;
        static const sRGB_color AntiqueWhite3;
        static const sRGB_color AntiqueWhite4;
        static const sRGB_color Aquamarine;
        static const sRGB_color Aquamarine1;
        static const sRGB_color Aquamarine2;
        static const sRGB_color Aquamarine3;
        static const sRGB_color Aquamarine4;
        static const sRGB_color Azure;
        static const sRGB_color Azure1;
        static const sRGB_color Azure2;
        static const sRGB_color Azure3;
        static const sRGB_color Azure4;
        static const sRGB_color Beige;
        static const sRGB_color Bisque;
        static const sRGB_color Bisque1;
        static const sRGB_color Bisque2;
        static const sRGB_color Bisque3;
        static const sRGB_color Bisque4;
        static const sRGB_color Black;
        static const sRGB_color BlanchedAlmond;
        static const sRGB_color Blue;
        static const sRGB_color Blue1;
        static const sRGB_color Blue2;
        static const sRGB_color Blue3;
        static const sRGB_color Blue4;
        static const sRGB_color BlueViolet;
        static const sRGB_color Brown;
        static const sRGB_color Brown1;
        static const sRGB_color Brown2;
        static const sRGB_color Brown3;
        static const sRGB_color Brown4;
        static const sRGB_color Burlywood;
        static const sRGB_color Burlywood1;
        static const sRGB_color Burlywood2;
        static const sRGB_color Burlywood3;
        static const sRGB_color Burlywood4;
        static const sRGB_color CadetBlue;
        static const sRGB_color CadetBlue1;
        static const sRGB_color CadetBlue2;
        static const sRGB_color CadetBlue3;
        static const sRGB_color CadetBlue4;
        static const sRGB_color Chartreuse;
        static const sRGB_color Chartreuse1;
        static const sRGB_color Chartreuse2;
        static const sRGB_color Chartreuse3;
        static const sRGB_color Chartreuse4;
        static const sRGB_color Chocolate;
        static const sRGB_color Chocolate1;
        static const sRGB_color Chocolate2;
        static const sRGB_color Chocolate3;
        static const sRGB_color Chocolate4;
        static const sRGB_color Coral;
        static const sRGB_color Coral1;
        static const sRGB_color Coral2;
        static const sRGB_color Coral3;
        static const sRGB_color Coral4;
        static const sRGB_color CornflowerBlue;
        static const sRGB_color Cornsilk;
        static const sRGB_color Cornsilk1;
        static const sRGB_color Cornsilk2;
        static const sRGB_color Cornsilk3;
        static const sRGB_color Cornsilk4;
        static const sRGB_color Cyan;
        static const sRGB_color Cyan1;
        static const sRGB_color Cyan2;
        static const sRGB_color Cyan3;
        static const sRGB_color Cyan4;
        static const sRGB_color DarkBlue;
        static const sRGB_color DarkCyan;
        static const sRGB_color DarkGoldenrod;
        static const sRGB_color DarkGoldenrod1;
        static const sRGB_color DarkGoldenrod2;
        static const sRGB_color DarkGoldenrod3;
        static const sRGB_color DarkGoldenrod4;
        static const sRGB_color DarkGray;
        static const sRGB_color DarkGreen;
        static const sRGB_color DarkGrey;
        static const sRGB_color DarkKhaki;
        static const sRGB_color DarkMagenta;
        static const sRGB_color DarkOliveGreen;
        static const sRGB_color DarkOliveGreen1;
        static const sRGB_color DarkOliveGreen2;
        static const sRGB_color DarkOliveGreen3;
        static const sRGB_color DarkOliveGreen4;
        static const sRGB_color DarkOrange;
        static const sRGB_color DarkOrange1;
        static const sRGB_color DarkOrange2;
        static const sRGB_color DarkOrange3;
        static const sRGB_color DarkOrange4;
        static const sRGB_color DarkOrchid;
        static const sRGB_color DarkOrchid1;
        static const sRGB_color DarkOrchid2;
        static const sRGB_color DarkOrchid3;
        static const sRGB_color DarkOrchid4;
        static const sRGB_color DarkRed;
        static const sRGB_color DarkSalmon;
        static const sRGB_color DarkSeaGreen;
        static const sRGB_color DarkSeaGreen1;
        static const sRGB_color DarkSeaGreen2;
        static const sRGB_color DarkSeaGreen3;
        static const sRGB_color DarkSeaGreen4;
        static const sRGB_color DarkSlateBlue;
        static const sRGB_color DarkSlateGray;
        static const sRGB_color DarkSlateGray1;
        static const sRGB_color DarkSlateGray2;
        static const sRGB_color DarkSlateGray3;
        static const sRGB_color DarkSlateGray4;
        static const sRGB_color DarkSlateGrey;
        static const sRGB_color DarkTurquoise;
        static const sRGB_color DarkViolet;
        static const sRGB_color DebianRed;
        static const sRGB_color DeepPink;
        static const sRGB_color DeepPink1;
        static const sRGB_color DeepPink2;
        static const sRGB_color DeepPink3;
        static const sRGB_color DeepPink4;
        static const sRGB_color DeepSkyBlue;
        static const sRGB_color DeepSkyBlue1;
        static const sRGB_color DeepSkyBlue2;
        static const sRGB_color DeepSkyBlue3;
        static const sRGB_color DeepSkyBlue4;
        static const sRGB_color DimGray;
        static const sRGB_color DimGrey;
        static const sRGB_color DodgerBlue;
        static const sRGB_color DodgerBlue1;
        static const sRGB_color DodgerBlue2;
        static const sRGB_color DodgerBlue3;
        static const sRGB_color DodgerBlue4;
        static const sRGB_color Firebrick;
        static const sRGB_color Firebrick1;
        static const sRGB_color Firebrick2;
        static const sRGB_color Firebrick3;
        static const sRGB_color Firebrick4;
        static const sRGB_color FloralWhite;
        static const sRGB_color ForestGreen;
        static const sRGB_color Gainsboro;
        static const sRGB_color GhostWhite;
        static const sRGB_color Gold;
        static const sRGB_color Gold1;
        static const sRGB_color Gold2;
        static const sRGB_color Gold3;
        static const sRGB_color Gold4;
        static const sRGB_color Goldenrod;
        static const sRGB_color Goldenrod1;
        static const sRGB_color Goldenrod2;
        static const sRGB_color Goldenrod3;
        static const sRGB_color Goldenrod4;
        static const sRGB_color Gray;
        static const sRGB_color Gray0;
        static const sRGB_color Gray1;
        static const sRGB_color Gray10;
        static const sRGB_color Gray100;
        static const sRGB_color Gray11;
        static const sRGB_color Gray12;
        static const sRGB_color Gray13;
        static const sRGB_color Gray14;
        static const sRGB_color Gray15;
        static const sRGB_color Gray16;
        static const sRGB_color Gray17;
        static const sRGB_color Gray18;
        static const sRGB_color Gray19;
        static const sRGB_color Gray2;
        static const sRGB_color Gray20;
        static const sRGB_color Gray21;
        static const sRGB_color Gray22;
        static const sRGB_color Gray23;
        static const sRGB_color Gray24;
        static const sRGB_color Gray25;
        static const sRGB_color Gray26;
        static const sRGB_color Gray27;
        static const sRGB_color Gray28;
        static const sRGB_color Gray29;
        static const sRGB_color Gray3;
        static const sRGB_color Gray30;
        static const sRGB_color Gray31;
        static const sRGB_color Gray32;
        static const sRGB_color Gray33;
        static const sRGB_color Gray34;
        static const sRGB_color Gray35;
        static const sRGB_color Gray36;
        static const sRGB_color Gray37;
        static const sRGB_color Gray38;
        static const sRGB_color Gray39;
        static const sRGB_color Gray4;
        static const sRGB_color Gray40;
        static const sRGB_color Gray41;
        static const sRGB_color Gray42;
        static const sRGB_color Gray43;
        static const sRGB_color Gray44;
        static const sRGB_color Gray45;
        static const sRGB_color Gray46;
        static const sRGB_color Gray47;
        static const sRGB_color Gray48;
        static const sRGB_color Gray49;
        static const sRGB_color Gray5;
        static const sRGB_color Gray50;
        static const sRGB_color Gray51;
        static const sRGB_color Gray52;
        static const sRGB_color Gray53;
        static const sRGB_color Gray54;
        static const sRGB_color Gray55;
        static const sRGB_color Gray56;
        static const sRGB_color Gray57;
        static const sRGB_color Gray58;
        static const sRGB_color Gray59;
        static const sRGB_color Gray6;
        static const sRGB_color Gray60;
        static const sRGB_color Gray61;
        static const sRGB_color Gray62;
        static const sRGB_color Gray63;
        static const sRGB_color Gray64;
        static const sRGB_color Gray65;
        static const sRGB_color Gray66;
        static const sRGB_color Gray67;
        static const sRGB_color Gray68;
        static const sRGB_color Gray69;
        static const sRGB_color Gray7;
        static const sRGB_color Gray70;
        static const sRGB_color Gray71;
        static const sRGB_color Gray72;
        static const sRGB_color Gray73;
        static const sRGB_color Gray74;
        static const sRGB_color Gray75;
        static const sRGB_color Gray76;
        static const sRGB_color Gray77;
        static const sRGB_color Gray78;
        static const sRGB_color Gray79;
        static const sRGB_color Gray8;
        static const sRGB_color Gray80;
        static const sRGB_color Gray81;
        static const sRGB_color Gray82;
        static const sRGB_color Gray83;
        static const sRGB_color Gray84;
        static const sRGB_color Gray85;
        static const sRGB_color Gray86;
        static const sRGB_color Gray87;
        static const sRGB_color Gray88;
        static const sRGB_color Gray89;
        static const sRGB_color Gray9;
        static const sRGB_color Gray90;
        static const sRGB_color Gray91;
        static const sRGB_color Gray92;
        static const sRGB_color Gray93;
        static const sRGB_color Gray94;
        static const sRGB_color Gray95;
        static const sRGB_color Gray96;
        static const sRGB_color Gray97;
        static const sRGB_color Gray98;
        static const sRGB_color Gray99;
        static const sRGB_color Green;
        static const sRGB_color Green1;
        static const sRGB_color Green2;
        static const sRGB_color Green3;
        static const sRGB_color Green4;
        static const sRGB_color GreenYellow;
        static const sRGB_color Grey;
        static const sRGB_color Grey0;
        static const sRGB_color Grey1;
        static const sRGB_color Grey10;
        static const sRGB_color Grey100;
        static const sRGB_color Grey11;
        static const sRGB_color Grey12;
        static const sRGB_color Grey13;
        static const sRGB_color Grey14;
        static const sRGB_color Grey15;
        static const sRGB_color Grey16;
        static const sRGB_color Grey17;
        static const sRGB_color Grey18;
        static const sRGB_color Grey19;
        static const sRGB_color Grey2;
        static const sRGB_color Grey20;
        static const sRGB_color Grey21;
        static const sRGB_color Grey22;
        static const sRGB_color Grey23;
        static const sRGB_color Grey24;
        static const sRGB_color Grey25;
        static const sRGB_color Grey26;
        static const sRGB_color Grey27;
        static const sRGB_color Grey28;
        static const sRGB_color Grey29;
        static const sRGB_color Grey3;
        static const sRGB_color Grey30;
        static const sRGB_color Grey31;
        static const sRGB_color Grey32;
        static const sRGB_color Grey33;
        static const sRGB_color Grey34;
        static const sRGB_color Grey35;
        static const sRGB_color Grey36;
        static const sRGB_color Grey37;
        static const sRGB_color Grey38;
        static const sRGB_color Grey39;
        static const sRGB_color Grey4;
        static const sRGB_color Grey40;
        static const sRGB_color Grey41;
        static const sRGB_color Grey42;
        static const sRGB_color Grey43;
        static const sRGB_color Grey44;
        static const sRGB_color Grey45;
        static const sRGB_color Grey46;
        static const sRGB_color Grey47;
        static const sRGB_color Grey48;
        static const sRGB_color Grey49;
        static const sRGB_color Grey5;
        static const sRGB_color Grey50;
        static const sRGB_color Grey51;
        static const sRGB_color Grey52;
        static const sRGB_color Grey53;
        static const sRGB_color Grey54;
        static const sRGB_color Grey55;
        static const sRGB_color Grey56;
        static const sRGB_color Grey57;
        static const sRGB_color Grey58;
        static const sRGB_color Grey59;
        static const sRGB_color Grey6;
        static const sRGB_color Grey60;
        static const sRGB_color Grey61;
        static const sRGB_color Grey62;
        static const sRGB_color Grey63;
        static const sRGB_color Grey64;
        static const sRGB_color Grey65;
        static const sRGB_color Grey66;
        static const sRGB_color Grey67;
        static const sRGB_color Grey68;
        static const sRGB_color Grey69;
        static const sRGB_color Grey7;
        static const sRGB_color Grey70;
        static const sRGB_color Grey71;
        static const sRGB_color Grey72;
        static const sRGB_color Grey73;
        static const sRGB_color Grey74;
        static const sRGB_color Grey75;
        static const sRGB_color Grey76;
        static const sRGB_color Grey77;
        static const sRGB_color Grey78;
        static const sRGB_color Grey79;
        static const sRGB_color Grey8;
        static const sRGB_color Grey80;
        static const sRGB_color Grey81;
        static const sRGB_color Grey82;
        static const sRGB_color Grey83;
        static const sRGB_color Grey84;
        static const sRGB_color Grey85;
        static const sRGB_color Grey86;
        static const sRGB_color Grey87;
        static const sRGB_color Grey88;
        static const sRGB_color Grey89;
        static const sRGB_color Grey9;
        static const sRGB_color Grey90;
        static const sRGB_color Grey91;
        static const sRGB_color Grey92;
        static const sRGB_color Grey93;
        static const sRGB_color Grey94;
        static const sRGB_color Grey95;
        static const sRGB_color Grey96;
        static const sRGB_color Grey97;
        static const sRGB_color Grey98;
        static const sRGB_color Grey99;
        static const sRGB_color Honeydew;
        static const sRGB_color Honeydew1;
        static const sRGB_color Honeydew2;
        static const sRGB_color Honeydew3;
        static const sRGB_color Honeydew4;
        static const sRGB_color HotPink;
        static const sRGB_color HotPink1;
        static const sRGB_color HotPink2;
        static const sRGB_color HotPink3;
        static const sRGB_color HotPink4;
        static const sRGB_color IndianRed;
        static const sRGB_color IndianRed1;
        static const sRGB_color IndianRed2;
        static const sRGB_color IndianRed3;
        static const sRGB_color IndianRed4;
        static const sRGB_color Ivory;
        static const sRGB_color Ivory1;
        static const sRGB_color Ivory2;
        static const sRGB_color Ivory3;
        static const sRGB_color Ivory4;
        static const sRGB_color Khaki;
        static const sRGB_color Khaki1;
        static const sRGB_color Khaki2;
        static const sRGB_color Khaki3;
        static const sRGB_color Khaki4;
        static const sRGB_color Lavender;
        static const sRGB_color LavenderBlush;
        static const sRGB_color LavenderBlush1;
        static const sRGB_color LavenderBlush2;
        static const sRGB_color LavenderBlush3;
        static const sRGB_color LavenderBlush4;
        static const sRGB_color LawnGreen;
        static const sRGB_color LemonChiffon;
        static const sRGB_color LemonChiffon1;
        static const sRGB_color LemonChiffon2;
        static const sRGB_color LemonChiffon3;
        static const sRGB_color LemonChiffon4;
        static const sRGB_color LightBlue;
        static const sRGB_color LightBlue1;
        static const sRGB_color LightBlue2;
        static const sRGB_color LightBlue3;
        static const sRGB_color LightBlue4;
        static const sRGB_color LightCoral;
        static const sRGB_color LightCyan;
        static const sRGB_color LightCyan1;
        static const sRGB_color LightCyan2;
        static const sRGB_color LightCyan3;
        static const sRGB_color LightCyan4;
        static const sRGB_color LightGoldenrod;
        static const sRGB_color LightGoldenrod1;
        static const sRGB_color LightGoldenrod2;
        static const sRGB_color LightGoldenrod3;
        static const sRGB_color LightGoldenrod4;
        static const sRGB_color LightGoldenrodYellow;
        static const sRGB_color LightGray;
        static const sRGB_color LightGreen;
        static const sRGB_color LightGrey;
        static const sRGB_color LightPink;
        static const sRGB_color LightPink1;
        static const sRGB_color LightPink2;
        static const sRGB_color LightPink3;
        static const sRGB_color LightPink4;
        static const sRGB_color LightSalmon;
        static const sRGB_color LightSalmon1;
        static const sRGB_color LightSalmon2;
        static const sRGB_color LightSalmon3;
        static const sRGB_color LightSalmon4;
        static const sRGB_color LightSeaGreen;
        static const sRGB_color LightSkyBlue;
        static const sRGB_color LightSkyBlue1;
        static const sRGB_color LightSkyBlue2;
        static const sRGB_color LightSkyBlue3;
        static const sRGB_color LightSkyBlue4;
        static const sRGB_color LightSlateBlue;
        static const sRGB_color LightSlateGray;
        static const sRGB_color LightSlateGrey;
        static const sRGB_color LightSteelBlue;
        static const sRGB_color LightSteelBlue1;
        static const sRGB_color LightSteelBlue2;
        static const sRGB_color LightSteelBlue3;
        static const sRGB_color LightSteelBlue4;
        static const sRGB_color LightYellow;
        static const sRGB_color LightYellow1;
        static const sRGB_color LightYellow2;
        static const sRGB_color LightYellow3;
        static const sRGB_color LightYellow4;
        static const sRGB_color LimeGreen;
        static const sRGB_color Linen;
        static const sRGB_color Magenta;
        static const sRGB_color Magenta1;
        static const sRGB_color Magenta2;
        static const sRGB_color Magenta3;
        static const sRGB_color Magenta4;
        static const sRGB_color Maroon;
        static const sRGB_color Maroon1;
        static const sRGB_color Maroon2;
        static const sRGB_color Maroon3;
        static const sRGB_color Maroon4;
        static const sRGB_color MediumAquamarine;
        static const sRGB_color MediumBlue;
        static const sRGB_color MediumOrchid;
        static const sRGB_color MediumOrchid1;
        static const sRGB_color MediumOrchid2;
        static const sRGB_color MediumOrchid3;
        static const sRGB_color MediumOrchid4;
        static const sRGB_color MediumPurple;
        static const sRGB_color MediumPurple1;
        static const sRGB_color MediumPurple2;
        static const sRGB_color MediumPurple3;
        static const sRGB_color MediumPurple4;
        static const sRGB_color MediumSeaGreen;
        static const sRGB_color MediumSlateBlue;
        static const sRGB_color MediumSpringGreen;
        static const sRGB_color MediumTurquoise;
        static const sRGB_color MediumVioletRed;
        static const sRGB_color MidnightBlue;
        static const sRGB_color MintCream;
        static const sRGB_color MistyRose;
        static const sRGB_color MistyRose1;
        static const sRGB_color MistyRose2;
        static const sRGB_color MistyRose3;
        static const sRGB_color MistyRose4;
        static const sRGB_color Moccasin;
        static const sRGB_color NavajoWhite;
        static const sRGB_color NavajoWhite1;
        static const sRGB_color NavajoWhite2;
        static const sRGB_color NavajoWhite3;
        static const sRGB_color NavajoWhite4;
        static const sRGB_color Navy;
        static const sRGB_color NavyBlue;
        static const sRGB_color OldLace;
        static const sRGB_color OliveDrab;
        static const sRGB_color OliveDrab1;
        static const sRGB_color OliveDrab2;
        static const sRGB_color OliveDrab3;
        static const sRGB_color OliveDrab4;
        static const sRGB_color Orange;
        static const sRGB_color Orange1;
        static const sRGB_color Orange2;
        static const sRGB_color Orange3;
        static const sRGB_color Orange4;
        static const sRGB_color OrangeRed;
        static const sRGB_color OrangeRed1;
        static const sRGB_color OrangeRed2;
        static const sRGB_color OrangeRed3;
        static const sRGB_color OrangeRed4;
        static const sRGB_color Orchid;
        static const sRGB_color Orchid1;
        static const sRGB_color Orchid2;
        static const sRGB_color Orchid3;
        static const sRGB_color Orchid4;
        static const sRGB_color PaleGoldenrod;
        static const sRGB_color PaleGreen;
        static const sRGB_color PaleGreen1;
        static const sRGB_color PaleGreen2;
        static const sRGB_color PaleGreen3;
        static const sRGB_color PaleGreen4;
        static const sRGB_color PaleTurquoise;
        static const sRGB_color PaleTurquoise1;
        static const sRGB_color PaleTurquoise2;
        static const sRGB_color PaleTurquoise3;
        static const sRGB_color PaleTurquoise4;
        static const sRGB_color PaleVioletRed;
        static const sRGB_color PaleVioletRed1;
        static const sRGB_color PaleVioletRed2;
        static const sRGB_color PaleVioletRed3;
        static const sRGB_color PaleVioletRed4;
        static const sRGB_color PapayaWhip;
        static const sRGB_color PeachPuff;
        static const sRGB_color PeachPuff1;
        static const sRGB_color PeachPuff2;
        static const sRGB_color PeachPuff3;
        static const sRGB_color PeachPuff4;
        static const sRGB_color Peru;
        static const sRGB_color Pink;
        static const sRGB_color Pink1;
        static const sRGB_color Pink2;
        static const sRGB_color Pink3;
        static const sRGB_color Pink4;
        static const sRGB_color Plum;
        static const sRGB_color Plum1;
        static const sRGB_color Plum2;
        static const sRGB_color Plum3;
        static const sRGB_color Plum4;
        static const sRGB_color PowderBlue;
        static const sRGB_color Purple;
        static const sRGB_color Purple1;
        static const sRGB_color Purple2;
        static const sRGB_color Purple3;
        static const sRGB_color Purple4;
        static const sRGB_color Red;
        static const sRGB_color Red1;
        static const sRGB_color Red2;
        static const sRGB_color Red3;
        static const sRGB_color Red4;
        static const sRGB_color RosyBrown;
        static const sRGB_color RosyBrown1;
        static const sRGB_color RosyBrown2;
        static const sRGB_color RosyBrown3;
        static const sRGB_color RosyBrown4;
        static const sRGB_color RoyalBlue;
        static const sRGB_color RoyalBlue1;
        static const sRGB_color RoyalBlue2;
        static const sRGB_color RoyalBlue3;
        static const sRGB_color RoyalBlue4;
        static const sRGB_color SaddleBrown;
        static const sRGB_color Salmon;
        static const sRGB_color Salmon1;
        static const sRGB_color Salmon2;
        static const sRGB_color Salmon3;
        static const sRGB_color Salmon4;
        static const sRGB_color SandyBrown;
        static const sRGB_color SeaGreen;
        static const sRGB_color SeaGreen1;
        static const sRGB_color SeaGreen2;
        static const sRGB_color SeaGreen3;
        static const sRGB_color SeaGreen4;
        static const sRGB_color Seashell;
        static const sRGB_color Seashell1;
        static const sRGB_color Seashell2;
        static const sRGB_color Seashell3;
        static const sRGB_color Seashell4;
        static const sRGB_color Sienna;
        static const sRGB_color Sienna1;
        static const sRGB_color Sienna2;
        static const sRGB_color Sienna3;
        static const sRGB_color Sienna4;
        static const sRGB_color SkyBlue;
        static const sRGB_color SkyBlue1;
        static const sRGB_color SkyBlue2;
        static const sRGB_color SkyBlue3;
        static const sRGB_color SkyBlue4;
        static const sRGB_color SlateBlue;
        static const sRGB_color SlateBlue1;
        static const sRGB_color SlateBlue2;
        static const sRGB_color SlateBlue3;
        static const sRGB_color SlateBlue4;
        static const sRGB_color SlateGray;
        static const sRGB_color SlateGray1;
        static const sRGB_color SlateGray2;
        static const sRGB_color SlateGray3;
        static const sRGB_color SlateGray4;
        static const sRGB_color SlateGrey;
        static const sRGB_color Snow;
        static const sRGB_color Snow1;
        static const sRGB_color Snow2;
        static const sRGB_color Snow3;
        static const sRGB_color Snow4;
        static const sRGB_color SpringGreen;
        static const sRGB_color SpringGreen1;
        static const sRGB_color SpringGreen2;
        static const sRGB_color SpringGreen3;
        static const sRGB_color SpringGreen4;
        static const sRGB_color SteelBlue;
        static const sRGB_color SteelBlue1;
        static const sRGB_color SteelBlue2;
        static const sRGB_color SteelBlue3;
        static const sRGB_color SteelBlue4;
        static const sRGB_color Tan;
        static const sRGB_color Tan1;
        static const sRGB_color Tan2;
        static const sRGB_color Tan3;
        static const sRGB_color Tan4;
        static const sRGB_color Thistle;
        static const sRGB_color Thistle1;
        static const sRGB_color Thistle2;
        static const sRGB_color Thistle3;
        static const sRGB_color Thistle4;
        static const sRGB_color Tomato;
        static const sRGB_color Tomato1;
        static const sRGB_color Tomato2;
        static const sRGB_color Tomato3;
        static const sRGB_color Tomato4;
        static const sRGB_color Turquoise;
        static const sRGB_color Turquoise1;
        static const sRGB_color Turquoise2;
        static const sRGB_color Turquoise3;
        static const sRGB_color Turquoise4;
        static const sRGB_color Violet;
        static const sRGB_color VioletRed;
        static const sRGB_color VioletRed1;
        static const sRGB_color VioletRed2;
        static const sRGB_color VioletRed3;
        static const sRGB_color VioletRed4;
        static const sRGB_color Wheat;
        static const sRGB_color Wheat1;
        static const sRGB_color Wheat2;
        static const sRGB_color Wheat3;
        static const sRGB_color Wheat4;
        static const sRGB_color White;
        static const sRGB_color WhiteSmoke;
        static const sRGB_color Yellow;
        static const sRGB_color Yellow1;
        static const sRGB_color Yellow2;
        static const sRGB_color Yellow3;
        static const sRGB_color Yellow4;
        static const sRGB_color YellowGreen;
    private:
        static const argb AlphaShift = 24;
        static const argb RedShift = 16;
        static const argb GreenShift = 8;
        static const argb BlueShift = 0;
        // construction
    public:
        sRGB_color();
        sRGB_color(const sRGB_color& aOther);
        explicit sRGB_color(const vec3& aBaseNoAlpha);
        explicit sRGB_color(const vec4& aBase);
        sRGB_color(view_component aRed, view_component aGreen, view_component aBlue, view_component aAlpha = 0xFF);
        sRGB_color(int32_t aRed, int32_t aGreen, int32_t aBlue, int32_t aAlpha = 0xFF) :
            sRGB_color{ static_cast<view_component>(aRed), static_cast<view_component>(aGreen), static_cast<view_component>(aBlue), static_cast<view_component>(aAlpha) } {}
        sRGB_color(uint32_t aRed, uint32_t aGreen, uint32_t aBlue, uint32_t aAlpha = 0xFF) :
            sRGB_color{ static_cast<view_component>(aRed), static_cast<view_component>(aGreen), static_cast<view_component>(aBlue), static_cast<view_component>(aAlpha) } {}
        sRGB_color(scalar aRed, scalar aGreen, scalar aBlue, scalar aAlpha = 1.0) :
            sRGB_color{ convert<view_component>(aRed), convert<view_component>(aGreen), convert<view_component>(aBlue), convert<view_component>(aAlpha) } {}
        explicit sRGB_color(argb aValue);
        explicit sRGB_color(const linear_color& aLinear);
        sRGB_color(const std::string& aTextValue);
        // assignment
    public:
        sRGB_color& operator=(const base_type& aOther);
         // operations
    public:
        static sRGB_color from_argb(argb aValue);
        static sRGB_color from_linear(const linear_color& aLinear);
        static sRGB_color from_hsl(scalar aHue, scalar aSaturation, scalar aLightness, scalar aAlpha = 1.0);
        static sRGB_color from_hsv(scalar aHue, scalar aSaturation, scalar aValue, scalar aAlpha = 1.0);
        static std::optional<sRGB_color> from_name(const std::string& aName);
        argb as_argb() const;
        linear_color to_linear() const;
        hsl_color to_hsl() const;
        hsv_color to_hsv() const;
        scalar brightness() const;
        scalar intensity() const;
        scalar luma() const;
        bool similar_intensity(const sRGB_color& aOther, scalar aThreshold = 0.5);
        sRGB_color mid(const sRGB_color& aOther) const;
        bool light(scalar aThreshold = 0.50) const;
        bool dark(scalar aThreshold = 0.50) const;
        sRGB_color& lighten(view_component aDelta);
        sRGB_color& darken(view_component aDelta);
        sRGB_color lighter(view_component aDelta) const;
        sRGB_color darker(view_component aDelta) const;
        sRGB_color shade(view_component aDelta) const;
        sRGB_color unshade(view_component aDelta) const;
        sRGB_color monochrome() const;
        sRGB_color same_lightness_as(const sRGB_color& aOther) const;
        sRGB_color with_lightness(scalar aLightness) const;
        sRGB_color inverse() const;
        sRGB_color& operator+=(view_component aDelta);
        sRGB_color& operator-=(view_component aDelta);
        sRGB_color operator~() const;
        bool operator<(const sRGB_color& aOther) const;
        std::string to_string() const;
        std::string to_hex_string() const;
        vec4 to_vec4() const;
        vec4f to_vec4f() const;
    };

    inline sRGB_color operator+(const sRGB_color& aLeft, sRGB_color::view_component aRight)
    {
        sRGB_color ret = aLeft;
        ret += aRight;
        return ret;
    }

    inline sRGB_color operator-(const sRGB_color& aLeft, sRGB_color::view_component aRight)
    {
        sRGB_color ret = aLeft;
        ret -= aRight;
        return ret;
    }

    inline sRGB_color operator*(const sRGB_color& aLeft, scalar aCoefficient)
    {
        return sRGB_color{ aLeft.to_vec4() *= vec4 { aCoefficient, aCoefficient, aCoefficient, 1.0 } };
    }

    template <typename Elem, typename Traits>
    inline std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& aStream, const sRGB_color& aColor)
    {
        aStream << "[A: 0x" << std::hex << (int)aColor.alpha() <<
            ", R: 0x" << std::hex << (int)aColor.red() <<
            ", G: 0x" << std::hex << (int)aColor.green() <<
            ", B: 0x" << std::hex << (int)aColor.blue() << "]";
        return aStream;
    }

    enum class gradient_direction : uint32_t
    {
        Vertical,
        Horizontal,
        Diagonal,
        Rectangular,
        Radial
    };

    enum class gradient_shape : uint32_t
    {
        Ellipse,
        Circle
    };

    enum class gradient_size : uint32_t
    {
        ClosestSide,
        FarthestSide,
        ClosestCorner,
        FarthestCorner
    };
}

template <>
const neolib::enum_enumerators_t<neogfx::gradient_direction> neolib::enum_enumerators_v<neogfx::gradient_direction>
{
    declare_enum_string(neogfx::gradient_direction, Vertical)
    declare_enum_string(neogfx::gradient_direction, Horizontal)
    declare_enum_string(neogfx::gradient_direction, Diagonal)
    declare_enum_string(neogfx::gradient_direction, Rectangular)
    declare_enum_string(neogfx::gradient_direction, Radial)
};

template <>
const neolib::enum_enumerators_t<neogfx::gradient_shape> neolib::enum_enumerators_v<neogfx::gradient_shape>
{
    declare_enum_string(neogfx::gradient_shape, Ellipse)
    declare_enum_string(neogfx::gradient_shape, Circle)
};

template <>
const neolib::enum_enumerators_t<neogfx::gradient_size> neolib::enum_enumerators_v<neogfx::gradient_size>
{
    declare_enum_string(neogfx::gradient_size, ClosestSide)
    declare_enum_string(neogfx::gradient_size, FarthestSide)
    declare_enum_string(neogfx::gradient_size, ClosestCorner)
    declare_enum_string(neogfx::gradient_size, FarthestCorner)
};

namespace neogfx
{
    class gradient
    {
        // constants
    public:
        static const std::uint32_t MaxStops = 256;
        // types
    public:
        typedef neolib::variant<corner, scalar> orientation_type;
        typedef std::pair<scalar, sRGB_color> color_stop;
        typedef std::vector<color_stop> color_stop_list;
        typedef std::pair<scalar, sRGB_color::view_component> alpha_stop;
        typedef std::vector<alpha_stop> alpha_stop_list;
    public:
        struct bad_position : std::logic_error { bad_position() : std::logic_error("neogfx::gradient::bad_position") {} };
        // construction
    public:
        gradient();
        gradient(const gradient& aOther);
        explicit gradient(const sRGB_color& aColor);
        gradient(const sRGB_color& aColor, gradient_direction aDirection);
        gradient(const sRGB_color& aColor1, const sRGB_color& aColor2, gradient_direction aDirection = gradient_direction::Vertical);
        gradient(const color_stop_list& aColorStops, gradient_direction aDirection = gradient_direction::Vertical);
        gradient(const color_stop_list& aColorStops, const alpha_stop_list& aAlphaStops, gradient_direction aDirection = gradient_direction::Vertical);
        gradient(const gradient& aOther, const color_stop_list& aColorStops, const alpha_stop_list& aAlphaStops);
        gradient(const std::initializer_list<sRGB_color>& aColors, gradient_direction aDirection = gradient_direction::Vertical);
        // operations
    public:
        bool use_cache() const;
        void set_cache_usage(bool aUseCache);
        color_stop_list::const_iterator color_begin() const;
        color_stop_list::const_iterator color_end() const;
        alpha_stop_list::const_iterator alpha_begin() const;
        alpha_stop_list::const_iterator alpha_end() const;
        color_stop_list::iterator color_begin();
        color_stop_list::iterator color_end();
        alpha_stop_list::iterator alpha_begin();
        alpha_stop_list::iterator alpha_end();
        color_stop_list::iterator find_color_stop(scalar aPos, bool aToInsert = false);
        color_stop_list::iterator find_color_stop(scalar aPos, scalar aStart, scalar aEnd, bool aToInsert = false);
        alpha_stop_list::iterator find_alpha_stop(scalar aPos, bool aToInsert = false);
        alpha_stop_list::iterator find_alpha_stop(scalar aPos, scalar aStart, scalar aEnd, bool aToInsert = false);
        color_stop_list::iterator insert_color_stop(scalar aPos);
        color_stop_list::iterator insert_color_stop(scalar aPos, scalar aStart, scalar aEnd);
        alpha_stop_list::iterator insert_alpha_stop(scalar aPos);
        alpha_stop_list::iterator insert_alpha_stop(scalar aPos, scalar aStart, scalar aEnd);
        void erase_stop(color_stop_list::iterator aStop);
        void erase_stop(alpha_stop_list::iterator aStop);
        std::size_t color_stop_count() const;
        std::size_t alpha_stop_count() const;
        color_stop_list combined_stops() const;
        sRGB_color at(scalar aPos) const;
        sRGB_color at(scalar aPos, scalar aStart, scalar aEnd) const;
        sRGB_color color_at(scalar aPos) const;
        sRGB_color color_at(scalar aPos, scalar aStart, scalar aEnd) const;
        sRGB_color::view_component alpha_at(scalar aPos) const;
        sRGB_color::view_component alpha_at(scalar aPos, scalar aStart, scalar aEnd) const;
        gradient reversed() const;
        template <typename T>
        gradient with_alpha(T aAlpha, std::enable_if_t<!std::is_same_v<T, sRGB_color::view_component>, sfinae> = {})
        {
            return with_alpha(sRGB_color::convert<sRGB_color::view_component>(aAlpha));
        }
        gradient with_alpha(sRGB_color::view_component aAlpha) const;
        template <typename T>
        gradient with_combined_alpha(T aAlpha, std::enable_if_t<!std::is_same_v<T, sRGB_color::view_component>, sfinae> = {})
        {
            return with_combined_alpha(sRGB_color::convert<sRGB_color::view_component>(aAlpha));
        }
        gradient with_combined_alpha(sRGB_color::view_component aAlpha) const;
        gradient_direction direction() const;
        void set_direction(gradient_direction aDirection);
        gradient with_direction(gradient_direction aDirection) const;
        orientation_type orientation() const;
        void set_orientation(orientation_type aOrientation);
        gradient with_orientation(orientation_type aOrientation) const;
        gradient_shape shape() const;
        void set_shape(gradient_shape aShape);
        gradient with_shape(gradient_shape aShape) const;
        gradient_size size() const;
        void set_size(gradient_size aSize);
        gradient with_size(gradient_size aSize) const;
        const optional_vec2& exponents() const;
        void set_exponents(const optional_vec2& aExponents);
        gradient with_exponents(const optional_vec2& aExponents) const;
        const optional_point& center() const;
        void set_center(const optional_point& aCenter);
        gradient with_center(const optional_point& aCenter) const;
        scalar smoothness() const;
        void set_smoothness(scalar aSmoothness);
        gradient with_smoothness(scalar aSmoothness) const;
    public:
        bool operator==(const gradient& aOther) const;
        bool operator!=(const gradient& aOther) const;
        bool operator<(const gradient& aOther) const;
    public:
        static scalar normalized_position(scalar aPos, scalar aStart, scalar aEnd);
    private:
        const color_stop_list& color_stops() const;
        color_stop_list& color_stops();
        const alpha_stop_list& alpha_stops() const;
        alpha_stop_list& alpha_stops();
        void fix();
        // attributes
    private:
        bool iUseCache;
        color_stop_list iColorStops;
        alpha_stop_list iAlphaStops;
        gradient_direction iDirection;
        orientation_type iOrientation;
        gradient_shape iShape;
        gradient_size iSize;
        optional_vec2 iExponents;
        optional_point iCenter;
        scalar iSmoothness;
    };

    using color = sRGB_color;

    typedef std::optional<color> optional_color;
    typedef std::optional<gradient> optional_gradient;

    typedef neolib::variant<color, gradient> color_or_gradient;
    typedef std::optional<color_or_gradient> optional_color_or_gradient;
}