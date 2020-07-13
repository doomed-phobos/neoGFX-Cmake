// gradient_dialog.cpp
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

#include <neogfx/gui/dialog/gradient_dialog.hpp>
#include <neolib/core/string_utils.hpp>
#include <neolib/task/thread.hpp>
#include <neogfx/core/numerical.hpp>
#include <neogfx/gui/dialog/message_box.hpp>
#include <neogfx/app/file_dialog.hpp>

namespace neogfx
{
    namespace
    {
        inline gradient convert_gimp_gradient(const gradient& aGradient, const std::string& aPath)
        {
            // todo: midpoint support
            std::ifstream gimpGradient{ aPath };
            std::string line;
            std::getline(gimpGradient, line);
            std::getline(gimpGradient, line);
            std::getline(gimpGradient, line);
            struct segment
            {
                double start;
                double mid;
                double end;
                vec4 startRgba;
                vec4 endRgba;
            };
            std::optional<segment> s;
            gradient::color_stop_list colorStops;
            gradient::alpha_stop_list alphaStops;
            while (std::getline(gimpGradient, line))
            {
                neolib::vecarray<std::string, 13> bits;
                neolib::tokens(line, " "s, bits);
                if (bits.size() != 13)
                    continue;
                s = segment
                {
                    boost::lexical_cast<double>(bits[0]),
                    boost::lexical_cast<double>(bits[1]),
                    boost::lexical_cast<double>(bits[2]),
                    vec4{
                        boost::lexical_cast<double>(bits[3]),
                        boost::lexical_cast<double>(bits[4]),
                        boost::lexical_cast<double>(bits[5]),
                        boost::lexical_cast<double>(bits[6]),
                    },
                    vec4{
                        boost::lexical_cast<double>(bits[7]),
                        boost::lexical_cast<double>(bits[8]),
                        boost::lexical_cast<double>(bits[9]),
                        boost::lexical_cast<double>(bits[10]),
                    }
                };
                colorStops.emplace_back(s->start, vec3{ s->startRgba.xyz });
                alphaStops.emplace_back(s->start, static_cast<color::component>(s->startRgba[3] * 0xFF));
                colorStops.emplace_back(s->mid, lerp(vec3{ s->startRgba.xyz }, vec3{ s->endRgba.xyz }, 0.5));
                alphaStops.emplace_back(s->mid, static_cast<color::component>(lerp(s->startRgba[3], s->endRgba[3], 0.5) * 0xFF));
            }
            colorStops.emplace_back(s->end, vec3{ s->endRgba.xyz });
            alphaStops.emplace_back(s->end, static_cast<color::component>(s->endRgba[3] * 0xFF));
            return gradient{ aGradient, colorStops, alphaStops };
        }
    }

    void draw_alpha_background(i_graphics_context& aGc, const rect& aRect, dimension aAlphaPatternSize = 4.0);

    class gradient_dialog::preview_box : public framed_widget<>
    {
        typedef framed_widget<> base_type;
    public:
        preview_box(gradient_dialog& aOwner) :
            base_type(aOwner.iPreviewGroupBox.item_layout()),
            iOwner(aOwner),
            iAnimationTimer{ service<async_task>(), [this](neolib::callback_timer& aTimer)
            {
                iSink += surface().closed([&aTimer]() { aTimer.cancel(); });
                aTimer.again();
                animate();
            }, 10, true },
            iTracking{ false }
        {
            set_padding(neogfx::padding{});
        }
    public:
        virtual void paint(i_graphics_context& aGc) const
        {
            base_type::paint(aGc);
            auto const& cr = client_rect();
            draw_alpha_background(aGc, cr, 16.0_dip);
            aGc.fill_rect(cr, iOwner.gradient());
            if (iOwner.gradient().direction() == gradient_direction::Radial && iOwner.gradient().center() != optional_point{})
            {
                point const center{ cr.center().x + cr.width() / 2.0 * iOwner.gradient().center()->x, cr.center().y + cr.height() / 2.0 * iOwner.gradient().center()->y };
                auto const radius = 6.0_dip;
                auto const circumference = 2.0 * math::pi<double>() * radius;
                aGc.draw_circle(center, radius, pen{ color::White, 1.0_dip });
                aGc.line_stipple_on(8.0_dip, 0x5555, circumference * neolib::thread::program_elapsed_ms() / 1000.0);
                aGc.draw_circle(center, radius, pen{ color::Black, 1.0_dip });
                aGc.line_stipple_off();
            }
        }
    public:
        virtual void mouse_button_pressed(mouse_button aButton, const point& aPosition, key_modifiers_e aKeyModifiers)
        {
            base_type::mouse_button_pressed(aButton, aPosition, aKeyModifiers);
            if (aButton == mouse_button::Left)
            {
                select(aPosition - client_rect(false).top_left());
                iTracking = true;
            }
        }
        virtual void mouse_button_released(mouse_button aButton, const point& aPosition)
        {
            base_type::mouse_button_released(aButton, aPosition);
            if (!capturing())
                iTracking = false;
        }
        virtual void mouse_moved(const point& aPosition, key_modifiers_e aKeyModifiers)
        {
            if (iTracking)
                select(aPosition - client_rect(false).top_left());
        }
        void select(const point& aPosition)
        {
            auto cr = client_rect(false);
            point center{ aPosition.x / cr.width() * 2.0 - 1.0, aPosition.y / cr.height() * 2.0 - 1.0 };
            center = center.max(point{ -1.0, -1.0 }).min(point{ 1.0, 1.0 });
            iOwner.gradient_selector().set_gradient(iOwner.gradient().with_center(center));
        }
    private:
        void animate()
        {
            if (iOwner.gradient().direction() == gradient_direction::Radial && iOwner.gradient().center() != optional_point{})
            {
                rect cr = client_rect();
                point center{ cr.center().x + cr.width() / 2.0 * iOwner.gradient().center()->x, cr.center().y + cr.height() / 2.0 * iOwner.gradient().center()->y };
                update(rect{ center - point{ 10, 10 }, size{ 20, 20 } });
            }
        }
    private:
        gradient_dialog& iOwner;
        neolib::sink iSink;
        neolib::callback_timer iAnimationTimer;
        bool iTracking;
    };

    gradient_dialog::gradient_dialog(i_widget& aParent, const neogfx::gradient& aCurrentGradient) :
        dialog(aParent, "Select Gradient"_t, window_style::Dialog | window_style::Modal | window_style::TitleBar | window_style::Close),
        iLayout{ client_layout() }, iLayout2{ iLayout }, iLayout3{ iLayout2 }, iLayout4{ iLayout2 },
        iSelectorGroupBox{ iLayout3 },
        iGradientSelector{ *this, iSelectorGroupBox.item_layout(), aCurrentGradient },
        iLayout3_1{ iSelectorGroupBox.item_layout() },
        iReverse{ iLayout3_1, image{ ":/neogfx/resources/icons/reverse.png" } },
        iReversePartial{ iLayout3_1, image{ ":/neogfx/resources/icons/reversepartial.png" } },
        iHueSlider{ iLayout3_1 },
        iImport{ iLayout3_1, image{ ":/neogfx/resources/icons/open.png" } },
        iLayout3_2{ iLayout3, alignment::Top },
        iDirectionGroupBox{ iLayout3_2, "Direction"_t },
        iDirectionHorizontalRadioButton{ iDirectionGroupBox.item_layout(), "Horizontal"_t },
        iDirectionVerticalRadioButton{ iDirectionGroupBox.item_layout(), "Vertical"_t },
        iDirectionDiagonalRadioButton{ iDirectionGroupBox.item_layout(), "Diagonal"_t },
        iDirectionRectangularRadioButton{ iDirectionGroupBox.item_layout(), "Rectangular"_t },
        iDirectionRadialRadioButton{ iDirectionGroupBox.item_layout(), "Radial"_t },
        iSmoothnessGroupBox{ iLayout3_2, "Smoothness (%)"_t },
        iSmoothnessSpinBox{ iSmoothnessGroupBox.item_layout() },
        iSmoothnessSlider{ iSmoothnessGroupBox.item_layout() },
        iLayout5{ iLayout3 },
        iOrientationGroupBox{ iLayout5, "Orientation"_t },
        iStartingFromGroupBox{ iOrientationGroupBox.with_item_layout<horizontal_layout>(), "Starting From"_t },
        iTopLeftRadioButton{ iStartingFromGroupBox.item_layout(), "Top left corner"_t },
        iTopRightRadioButton{ iStartingFromGroupBox.item_layout(), "Top right corner"_t },
        iBottomRightRadioButton{ iStartingFromGroupBox.item_layout(), "Bottom right corner"_t },
        iBottomLeftRadioButton{ iStartingFromGroupBox.item_layout(), "Bottom left corner"_t },
        iAngleRadioButton{ iStartingFromGroupBox.item_layout(), "At a specific angle"_t },
        iLayout6 { iOrientationGroupBox.item_layout() },
        iAngleGroupBox{ iLayout6 },
        iAngle{ iAngleGroupBox.with_item_layout<grid_layout>(), "Angle:"_t },
        iAngleSpinBox{ iAngleGroupBox.item_layout() },
        iAngleSlider{ iAngleGroupBox.item_layout() },
        iSizeGroupBox{ iLayout5, "Size"_t },
        iSizeClosestSideRadioButton{ iSizeGroupBox.item_layout(), "Closest side"_t },
        iSizeFarthestSideRadioButton{ iSizeGroupBox.item_layout(), "Farthest side"_t },
        iSizeClosestCornerRadioButton{ iSizeGroupBox.item_layout(), "Closest corner"_t },
        iSizeFarthestCornerRadioButton{ iSizeGroupBox.item_layout(), "Farthest corner"_t },
        iShapeGroupBox{ iLayout5, "Shape"_t },
        iShapeEllipseRadioButton{ iShapeGroupBox.item_layout(), "Ellipse"_t },
        iShapeCircleRadioButton{ iShapeGroupBox.item_layout(), "Circle"_t },
        iExponentGroupBox{ iLayout5, "Exponents"_t },
        iLinkedExponents{ iExponentGroupBox.with_item_layout<grid_layout>(3, 2).add_span(grid_layout::cell_coordinates{0, 0}, grid_layout::cell_coordinates{1, 0}), "Linked"_t },
        iMExponent{ iExponentGroupBox.item_layout(), "m:"_t },
        iMExponentSpinBox{ iExponentGroupBox.item_layout() },
        iNExponent{ iExponentGroupBox.item_layout(), "n:"_t },
        iNExponentSpinBox{ iExponentGroupBox.item_layout() },
        iCenterGroupBox{ iLayout5, "Center"_t },
        iXCenter{ iCenterGroupBox.with_item_layout<grid_layout>(2, 2), "X:"_t },
        iXCenterSpinBox { iCenterGroupBox.item_layout() },
        iYCenter{ iCenterGroupBox.item_layout(), "Y:"_t },
        iYCenterSpinBox { iCenterGroupBox.item_layout() },
        iSpacer2{ iLayout5 },
        iSpacer3{ iLayout3 },
        iPreviewGroupBox{ iLayout4, "Preview"_t },
        iPreview{ new preview_box{*this} },
        iSpacer4{ iLayout4 },
        iUpdatingWidgets{ false },
        iIgnoreHueSliderChange{ false }
    {
        init();
    }

    gradient_dialog::~gradient_dialog()
    {
    }

    const gradient& gradient_dialog::gradient() const
    {
        return gradient_selector().gradient();
    }

    void gradient_dialog::set_gradient(const neogfx::gradient& aGradient)
    {
        gradient_selector().set_gradient(aGradient);
    }
    
    const gradient_widget& gradient_dialog::gradient_selector() const
    {
        return iGradientSelector;
    }

    gradient_widget& gradient_dialog::gradient_selector()
    {
        return iGradientSelector;
    }

    void gradient_dialog::init()
    {
        iLayout.set_padding(neogfx::padding{});
        iLayout2.set_padding(neogfx::padding{});
        iLayout3.set_padding(neogfx::padding{});
        iLayout5.set_alignment(alignment::Top);
        iReverse.image_widget().set_fixed_size(size{ 16_dip, 16_dip });
        iReverse.image_widget().set_image_color(service<i_app>().current_style().palette().color(color_role::Text));
        iReversePartial.image_widget().set_fixed_size(size{ 16_dip, 16_dip });
        iReversePartial.image_widget().set_image_color(service<i_app>().current_style().palette().color(color_role::Text));
        iHueSlider.set_minimum(0.0);
        iHueSlider.set_maximum(360.0);
        iHueSlider.set_step(1.0);
        iHueSlider.disable();
        neogfx::gradient::color_stop_list hues;
        for (uint32_t s = 0; s < neogfx::gradient::MaxStops; ++s)
        {
            double const pos = s / static_cast<double>((neogfx::gradient::MaxStops - 1));
            hues.emplace_back(pos, color::from_hsv(pos * 360.0, 1.0, 1.0));
        }
        iHueSlider.set_bar_color(neogfx::gradient{ hues, gradient_direction::Horizontal });
        iImport.image_widget().set_fixed_size(size{ 16_dip, 16_dip });
        iSmoothnessSpinBox.set_minimum(0.0);
        iSmoothnessSpinBox.set_maximum(100.0);
        iSmoothnessSpinBox.set_step(0.1);
        iSmoothnessSpinBox.set_format("%.1f");
        iSmoothnessSlider.set_minimum(0.0);
        iSmoothnessSlider.set_maximum(100.0);
        iSmoothnessSlider.set_step(0.1);
        iOrientationGroupBox.item_layout().set_alignment(alignment::Top);
        iAngleSpinBox.set_minimum(-360.0);
        iAngleSpinBox.set_maximum(360.0);
        iAngleSpinBox.set_step(0.1);
        iAngleSpinBox.set_format("%.1f");
        iAngleSlider.set_minimum(-360.0);
        iAngleSlider.set_maximum(360.0);
        iAngleSlider.set_step(0.1);
        iExponentGroupBox.set_checkable(true);
        iExponentGroupBox.item_layout().set_alignment(alignment::Right);
        iLinkedExponents.set_size_policy(size_constraint::Expanding);
        iLinkedExponents.set_checked(true);
        iMExponentSpinBox.set_minimum(0.0);
        iMExponentSpinBox.set_maximum(std::numeric_limits<double>::max());
        iMExponentSpinBox.set_step(0.1);
        iMExponentSpinBox.set_format("%.2f");
        iMExponentSpinBox.text_box().set_alignment(alignment::Right);
        iMExponentSpinBox.text_box().set_size_hint(size_hint{ "00.00" });
        iNExponentSpinBox.set_minimum(0.0);
        iNExponentSpinBox.set_maximum(std::numeric_limits<double>::max());
        iNExponentSpinBox.set_step(0.1);
        iNExponentSpinBox.set_format("%.2f");
        iNExponentSpinBox.text_box().set_alignment(alignment::Right);
        iNExponentSpinBox.text_box().set_size_hint(size_hint{ "00.00" });
        iCenterGroupBox.set_checkable(true);
        iXCenterSpinBox.set_minimum(-1.0);
        iXCenterSpinBox.set_maximum(1.0);
        iXCenterSpinBox.set_step(0.001);
        iXCenterSpinBox.set_format("%.3f");
        iXCenterSpinBox.text_box().set_alignment(alignment::Right);
        iXCenterSpinBox.text_box().set_size_hint(size_hint{ "-0.000" });
        iYCenterSpinBox.set_minimum(-1.0);
        iYCenterSpinBox.set_maximum(1.0);
        iYCenterSpinBox.set_step(0.001);
        iYCenterSpinBox.set_format("%.3f");
        iYCenterSpinBox.text_box().set_alignment(alignment::Right);
        iYCenterSpinBox.text_box().set_size_hint(size_hint{ "-0.000" });

        iGradientSelector.set_fixed_size(size{ 256.0_dip, iGradientSelector.minimum_size().cy });

        auto update_hue_selection = [this]()
        {
            neolib::scoped_flag scope{ iIgnoreHueSliderChange };
            iHueSlider.set_value(iGradientSelector.selected_color_stop()->second.to_hsv().hue());
            iHueSelection.clear();
            auto stopIter = iGradientSelector.gradient().color_begin();
            for (std::size_t stopIndex = 0; stopIndex < iGradientSelector.gradient().color_stop_count(); ++stopIndex, ++stopIter)
            {
                auto const& colorStop = *stopIter;
                auto d = (colorStop.second.to_hsv().hue() - iHueSlider.value());
                auto const tolerance_deg = 3.0; // todo: make this configurable
                if (std::abs(d) < tolerance_deg)
                    iHueSelection.emplace_back(stopIndex, d);
            }
        };

        iGradientSelector.GradientChanged([this, update_hue_selection]()
        { 
            if (iGradientSelector.selected_color_stop() != iGradientSelector.gradient().color_end())
                update_hue_selection();
            update_widgets();
        });

        iReverse.Clicked([this]()
        {
            set_gradient(gradient().reversed());
        });

        iReversePartial.Clicked([this]()
        {
            auto partiallyReversedGradient = gradient();
            for (auto colorStop = partiallyReversedGradient.color_begin(); colorStop != partiallyReversedGradient.color_end(); ++colorStop)
                colorStop->second = std::prev(gradient().color_end(), std::distance(partiallyReversedGradient.color_begin(), colorStop) + 1)->second;
            set_gradient(partiallyReversedGradient);
        });

        iGradientSelector.ColorStopSelected([this, update_hue_selection]()
        {
            iHueSlider.enable();
            update_hue_selection();
        });

        iGradientSelector.ColorStopDeselected([this]()
        {
            iHueSlider.disable();
            iHueSelection.clear();
        });

        iHueSlider.ValueChanged([this]()
        {
            if (iIgnoreHueSliderChange)
                return;
            neolib::scoped_flag scope{ iIgnoreHueSliderChange };
            auto newGradient = iGradientSelector.gradient();
            for (auto const& hs : iHueSelection)
            {
                auto& colorStop = *std::next(newGradient.color_begin(), hs.first);
                auto newColor = colorStop.second.to_hsv();
                newColor.set_hue(iHueSlider.value() + hs.second);
                colorStop.second = newColor.to_rgb<color>();
            }
            thread_local decltype(iHueSelection) hueSelectionCopy;
            hueSelectionCopy = iHueSelection;
            iGradientSelector.set_gradient(newGradient);
            iHueSelection = hueSelectionCopy;
        });

        iImport.Clicked([this]()
        {
            auto const imports = open_file_dialog(*this, file_dialog_spec{ "Import Gradients", {}, { "*.ggr" }, "Gradient Files (*.ggr)" }, true);
            if (imports)
            {
                // todo: populate swatch library
                try
                {
                    set_gradient(convert_gimp_gradient(gradient(), (*imports)[0]));
                }
                catch (...)
                {
                    message_box::error("Import Gradient", "Failed to import gradient(s)");
                }
            }
        });

        iSmoothnessSpinBox.ValueChanged([this]() { iGradientSelector.set_gradient(gradient().with_smoothness(iSmoothnessSpinBox.value() / 100.0)); });
        iSmoothnessSlider.ValueChanged([this]() { iGradientSelector.set_gradient(gradient().with_smoothness(iSmoothnessSlider.value() / 100.0)); });

        iDirectionHorizontalRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_direction(gradient_direction::Horizontal)); });
        iDirectionVerticalRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_direction(gradient_direction::Vertical)); });
        iDirectionDiagonalRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_direction(gradient_direction::Diagonal)); });
        iDirectionRectangularRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_direction(gradient_direction::Rectangular)); });
        iDirectionRadialRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_direction(gradient_direction::Radial)); });

        iTopLeftRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_orientation(corner::TopLeft)); });
        iTopRightRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_orientation(corner::TopRight)); });
        iBottomRightRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_orientation(corner::BottomRight)); });
        iBottomLeftRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_orientation(corner::BottomLeft)); });
        iAngleRadioButton.checked([this]() 
        { 
            if (!std::holds_alternative<double>(iGradientSelector.gradient().orientation())) 
                iGradientSelector.set_gradient(gradient().with_orientation(0.0)); 
        });

        iAngleSpinBox.ValueChanged([this]() { iGradientSelector.set_gradient(gradient().with_orientation(to_rad(iAngleSpinBox.value()))); });
        iAngleSlider.ValueChanged([this]() { iGradientSelector.set_gradient(gradient().with_orientation(to_rad(iAngleSlider.value()))); });

        iSizeClosestSideRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_size(gradient_size::ClosestSide)); });
        iSizeFarthestSideRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_size(gradient_size::FarthestSide)); });
        iSizeClosestCornerRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_size(gradient_size::ClosestCorner)); });
        iSizeFarthestCornerRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_size(gradient_size::FarthestCorner)); });

        iShapeEllipseRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_shape(gradient_shape::Ellipse)); });
        iShapeCircleRadioButton.checked([this]() { iGradientSelector.set_gradient(gradient().with_shape(gradient_shape::Circle)); });

        iExponentGroupBox.check_box().checked([this]() { iGradientSelector.set_gradient(gradient().with_exponents(vec2{2.0, 2.0})); });
        iExponentGroupBox.check_box().Unchecked([this]() { iGradientSelector.set_gradient(gradient().with_exponents(optional_vec2{})); });

        iLinkedExponents.checked([this]() { auto e = gradient().exponents(); if (e != std::nullopt) { iGradientSelector.set_gradient(gradient().with_exponents(vec2{ e->x, e->x })); } });

        iMExponentSpinBox.ValueChanged([this]()
        { 
            if (iUpdatingWidgets)
                return;
            auto e = gradient().exponents(); 
            if (e == std::nullopt) 
                e = vec2{}; 
            e->x = iMExponentSpinBox.value(); 
            if (iLinkedExponents.is_checked())
                e->y = e->x;
            iGradientSelector.set_gradient(gradient().with_exponents(e)); 
        });

        iNExponentSpinBox.ValueChanged([this]()
        {
            if (iUpdatingWidgets)
                return;
            auto e = gradient().exponents();
            if (e == std::nullopt)
                e = vec2{};
            e->y = iNExponentSpinBox.value();
            if (iLinkedExponents.is_checked())
                e->x = e->y;
            iGradientSelector.set_gradient(gradient().with_exponents(e));
        });

        iCenterGroupBox.check_box().Checked([this]() { iGradientSelector.set_gradient(gradient().center() ? gradient() : gradient().with_center(point{})); });
        iCenterGroupBox.check_box().Unchecked([this]() { iGradientSelector.set_gradient(gradient().with_center(optional_point{})); });

        iXCenterSpinBox.ValueChanged([this]() { auto c = gradient().center(); if (c == std::nullopt) c = point{}; c->x = iXCenterSpinBox.value(); iGradientSelector.set_gradient(gradient().with_center(c)); });
        iYCenterSpinBox.ValueChanged([this]() { auto c = gradient().center(); if (c == std::nullopt) c = point{}; c->y = iYCenterSpinBox.value(); iGradientSelector.set_gradient(gradient().with_center(c)); });

        iPreview->set_padding(neogfx::padding{});
        iPreview->set_fixed_size(size{ std::ceil(256.0_dip * 16.0 / 9.0), 256.0_dip });

        button_box().add_button(standard_button::Ok);
        button_box().add_button(standard_button::Cancel);
        
        update_widgets();

        layout().invalidate();
        center_on_parent();
        set_ready_to_render(true);
    }

    void gradient_dialog::update_widgets()
    {
        if (iUpdatingWidgets)
            return;
        neolib::scoped_flag sf{ iUpdatingWidgets };
        iSmoothnessSpinBox.set_value(gradient().smoothness() * 100.0);
        iSmoothnessSlider.set_value(gradient().smoothness() * 100.0);
        iDirectionHorizontalRadioButton.set_checked(gradient().direction() == gradient_direction::Horizontal);
        iDirectionVerticalRadioButton.set_checked(gradient().direction() == gradient_direction::Vertical);
        iDirectionDiagonalRadioButton.set_checked(gradient().direction() == gradient_direction::Diagonal);
        iDirectionRectangularRadioButton.set_checked(gradient().direction() == gradient_direction::Rectangular);
        iDirectionRadialRadioButton.set_checked(gradient().direction() == gradient_direction::Radial);
        iTopLeftRadioButton.set_checked(gradient().orientation() == gradient::orientation_type(corner::TopLeft));
        iTopRightRadioButton.set_checked(gradient().orientation() == gradient::orientation_type(corner::TopRight));
        iBottomRightRadioButton.set_checked(gradient().orientation() == gradient::orientation_type(corner::BottomRight));
        iBottomLeftRadioButton.set_checked(gradient().orientation() == gradient::orientation_type(corner::BottomLeft));
        iAngleRadioButton.set_checked(std::holds_alternative<double>(gradient().orientation()));
        iAngleSpinBox.set_value(std::holds_alternative<double>(gradient().orientation()) ? to_deg(static_variant_cast<double>(gradient().orientation())) : 0.0);
        iAngleSlider.set_value(std::holds_alternative<double>(gradient().orientation()) ? to_deg(static_variant_cast<double>(gradient().orientation())) : 0.0);
        iSizeClosestSideRadioButton.set_checked(gradient().size() == gradient_size::ClosestSide);
        iSizeFarthestSideRadioButton.set_checked(gradient().size() == gradient_size::FarthestSide);
        iSizeClosestCornerRadioButton.set_checked(gradient().size() == gradient_size::ClosestCorner);
        iSizeFarthestCornerRadioButton.set_checked(gradient().size() == gradient_size::FarthestCorner);
        iShapeEllipseRadioButton.set_checked(gradient().shape() == gradient_shape::Ellipse);
        iShapeCircleRadioButton.set_checked(gradient().shape() == gradient_shape::Circle);
        auto exponents = gradient().exponents();
        bool specifyExponents = (exponents != std::nullopt);
        iExponentGroupBox.check_box().set_checked(specifyExponents);
        if (specifyExponents)
        {
            iMExponentSpinBox.set_value(exponents->x, false);
            iNExponentSpinBox.set_value(exponents->y, false);
        }
        else
        {
            iMExponentSpinBox.text_box().set_text("");
            iNExponentSpinBox.text_box().set_text("");
        }
        iLinkedExponents.enable(specifyExponents);
        iMExponent.enable(specifyExponents);
        iMExponentSpinBox.enable(specifyExponents);
        iNExponent.enable(specifyExponents);
        iNExponentSpinBox.enable(specifyExponents);        
        auto const center = gradient().center();
        bool const specifyCenter = (center != std::nullopt);
        if (specifyCenter)
        {
            iXCenterSpinBox.set_value(center->x, false);
            iYCenterSpinBox.set_value(center->y, false);
        }
        else
        {
            iXCenterSpinBox.text_box().set_text("");
            iYCenterSpinBox.text_box().set_text("");
        }
        iCenterGroupBox.check_box().set_checked(specifyCenter);
        iXCenter.enable(specifyCenter);
        iXCenterSpinBox.enable(specifyCenter);
        iYCenter.enable(specifyCenter);
        iYCenterSpinBox.enable(specifyCenter);
        switch (gradient().direction())
        {
        case gradient_direction::Vertical:
        case gradient_direction::Horizontal:
        case gradient_direction::Rectangular:
            iOrientationGroupBox.hide();
            iSizeGroupBox.hide();
            iShapeGroupBox.hide();
            iExponentGroupBox.hide();
            iCenterGroupBox.hide();
            break;
        case gradient_direction::Diagonal:
            iOrientationGroupBox.show();
            iAngleGroupBox.show(std::holds_alternative<double>(gradient().orientation()));
            iSizeGroupBox.hide();
            iShapeGroupBox.hide();
            iExponentGroupBox.hide();
            iCenterGroupBox.hide();
            break;
        case gradient_direction::Radial:
            iOrientationGroupBox.hide();
            iSizeGroupBox.show();
            iShapeGroupBox.show();
            iExponentGroupBox.show(gradient().shape() == gradient_shape::Ellipse);
            iCenterGroupBox.show();
            break;
        }
        iPreview->update();
    }
}