#include <SFGUI/Engines/BREW.hpp>
#include <SFGUI/CheckButton.hpp>

namespace sfg {
namespace eng {

RenderQueue* BREW::CreateCheckButtonDrawable( SharedPtr<const CheckButton> check ) const {
	sf::Color border_color_light( GetProperty<sf::Color>( "BorderColor", check ) );
	sf::Color border_color_dark( border_color_light );
	sf::Color background_color( GetProperty<sf::Color>( "BackgroundColor", check ) );
	sf::Color color( GetProperty<sf::Color>( "Color", check ) );
	sf::Color check_color( GetProperty<sf::Color>( "CheckColor", check ) );
	int border_color_shift( GetProperty<int>( "BorderColorShift", check ) );
	float border_width( GetProperty<float>( "BorderWidth", check ) );
	float box_size( GetProperty<float>( "BoxSize", check ) );
	float spacing( GetProperty<float>( "Spacing", check ) );
	float check_size( std::min( box_size, GetProperty<float>( "CheckSize", check ) ) );
	const std::string& font_name( GetProperty<std::string>( "FontName", check ) );
	unsigned int font_size( GetProperty<unsigned int>( "FontSize", check ) );
	const sf::Font& font( *GetResourceManager().GetFont( font_name ) );
	RenderQueue* queue( new RenderQueue );

	ShiftBorderColors( border_color_light, border_color_dark, border_color_shift );

	queue->Add(
		new sf::Shape(
			sf::Shape::Rectangle(
				0.f,
				check->GetAllocation().Height / 2.f - box_size / 2.f,
				box_size,
				box_size,
				background_color
			)
		)
	);

	if( check->IsActive() ) {
		float diff( box_size - check_size );

		queue->Add(
			new sf::Shape(
				sf::Shape::Rectangle(
					diff / 2.f,
					check->GetAllocation().Height / 2.f - box_size / 2.f + diff / 2.f,
					box_size - diff,
					box_size - diff,
					check_color
				)
			)
		);
	}

	queue->Add(
		CreateBorder(
			sf::FloatRect(
				0.f,
				check->GetAllocation().Height / 2.f - box_size / 2.f,
				box_size,
				box_size
			),
			border_width,
			border_color_dark,
			border_color_light
		)
	);

	// Label.
	if( check->GetLabel().GetSize() > 0 ) {
		sf::Vector2f metrics = GetTextMetrics( check->GetLabel(), font, font_size );
		metrics.y = GetLineHeight( font, font_size );

		sf::Text* text( new sf::Text( check->GetLabel(), font, font_size ) );
		text->SetPosition(
			std::floor( box_size + spacing ),
			std::floor( check->GetAllocation().Height / 2.f - metrics.y / 2.f + .5f )
		);
		text->SetColor( color );
		queue->Add( text );
	}

	return queue;
}

}
}