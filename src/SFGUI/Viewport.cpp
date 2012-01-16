#include <SFGUI/Viewport.hpp>
#include <SFGUI/Context.hpp>
#include <cmath>

namespace sfg {

Viewport::Viewport( const Adjustment::Ptr& horizontal_adjustment, const Adjustment::Ptr& vertical_adjustment ) :
	Bin()
{
	OnSizeRequest.Connect( &Viewport::HandleRequisitionChange, this );

	SetHorizontalAdjustment( horizontal_adjustment );
	SetVerticalAdjustment( vertical_adjustment );

	m_viewport = Context::Get().GetRenderer().CreateViewport();
}

Viewport::Ptr Viewport::Create() {
	return Viewport::Create( Adjustment::Create(), Adjustment::Create() );
}

Viewport::Ptr Viewport::Create( const Adjustment::Ptr& horizontal_adjustment, const Adjustment::Ptr& vertical_adjustment ) {
	Viewport::Ptr ptr( new Viewport( horizontal_adjustment, vertical_adjustment ) );
	return ptr;
}

RenderQueue* Viewport::InvalidateImpl() const {
	m_viewport->source_origin.x = std::floor( m_horizontal_adjustment->GetValue() + .5f );
	m_viewport->source_origin.y = std::floor( m_vertical_adjustment->GetValue() + .5f );

	return 0;
}

sf::Vector2f Viewport::CalculateRequisition() {
	return sf::Vector2f( 0.f, 0.f );
}

void Viewport::HandleAllocationChange( const sf::FloatRect& /*old_allocation*/ ) {
	sf::FloatRect allocation = GetAllocation();

	m_viewport->size.x = std::floor( allocation.Width + .5f );
	m_viewport->size.y = std::floor( allocation.Height + .5f );
}

void Viewport::HandleAbsolutePositionChange() {
	sf::Vector2f position = Widget::GetAbsolutePosition();

	m_viewport->destination_origin.x = std::floor( position.x + .5f );
	m_viewport->destination_origin.y = std::floor( position.y + .5f );
}

void Viewport::HandleEvent( const sf::Event& event ) {
	// Ignore event when widget is not visible.
	if( !IsVisible() ) {
		return;
	}

	// Pass event to child
	if( GetChild() ) {
		float offset_x = ( -GetAllocation().Left + m_horizontal_adjustment->GetValue() );
		float offset_y = ( -GetAllocation().Top + m_vertical_adjustment->GetValue() );

		switch( event.Type ) {
		case sf::Event::MouseButtonPressed:
		case sf::Event::MouseButtonReleased: { // All MouseButton events
			if( !GetAllocation().Contains( static_cast<float>( event.MouseButton.X ), static_cast<float>( event.MouseButton.Y ) ) ) {
				break;
			}

			sf::Event altered_event( event );
			altered_event.MouseButton.X += static_cast<int>( offset_x );
			altered_event.MouseButton.Y += static_cast<int>( offset_y );

			GetChild()->HandleEvent( altered_event );
		} break;
		case sf::Event::MouseEntered:
		case sf::Event::MouseLeft:
		case sf::Event::MouseMoved: { // All MouseMove events
			if( !GetAllocation().Contains( static_cast<float>( event.MouseMove.X ), static_cast<float>( event.MouseMove.Y ) ) ) {
				// Nice hack to cause scrolledwindow children to get out of
				// prelight state when the mouse leaves the child allocation.
				sf::Event altered_event( event );
				altered_event.MouseMove.X = -1;
				altered_event.MouseMove.Y = -1;

				GetChild()->HandleEvent( altered_event );
				break;
			}

			sf::Event altered_event( event );
			altered_event.MouseMove.X += static_cast<int>( offset_x );
			altered_event.MouseMove.Y += static_cast<int>( offset_y );

			GetChild()->HandleEvent( altered_event );
		} break;
		case sf::Event::MouseWheelMoved: { // All MouseWheel events
			if( !GetAllocation().Contains( static_cast<float>( event.MouseWheel.X ), static_cast<float>( event.MouseWheel.Y ) ) ) {
				break;
			}

			sf::Event altered_event( event );
			altered_event.MouseWheel.X += static_cast<int>( offset_x );
			altered_event.MouseWheel.Y += static_cast<int>( offset_y );

			GetChild()->HandleEvent( altered_event );
		} break;
		default: { // Pass event unaltered if it is a non-mouse event
			GetChild()->HandleEvent( event );
		} break;
		}
	}
}

sf::Vector2f Viewport::GetAbsolutePosition() const {
	return sf::Vector2f( .0f, .0f );
}

const Adjustment::Ptr& Viewport::GetHorizontalAdjustment() const {
	return m_horizontal_adjustment;
}

void Viewport::SetHorizontalAdjustment( const Adjustment::Ptr& horizontal_adjustment ) {
	m_horizontal_adjustment = horizontal_adjustment;
	m_horizontal_adjustment->OnChange.Connect( &Viewport::UpdateView, this );
}

const Adjustment::Ptr& Viewport::GetVerticalAdjustment() const {
	return m_vertical_adjustment;
}

void Viewport::SetVerticalAdjustment( const Adjustment::Ptr& vertical_adjustment ) {
	m_vertical_adjustment = vertical_adjustment;
	m_vertical_adjustment->OnChange.Connect( &Viewport::UpdateView, this );
}

void Viewport::HandleRequisitionChange() {
	// A child just requested it's size. Because we are a viewport
	// and have a virtual screen we give it everything it wants.
	if( GetChild() ) {
		sf::FloatRect new_allocation = GetChild()->GetAllocation();
		new_allocation.Width = GetChild()->GetRequisition().x;
		new_allocation.Height = GetChild()->GetRequisition().y;
		GetChild()->SetAllocation( new_allocation );
	}
}

const std::string& Viewport::GetName() const {
	static const std::string name( "Viewport" );
	return name;
}

void Viewport::HandleAdd( const Widget::Ptr& child ) {
	Bin::HandleAdd( child );

	if( !IsChild( child ) ) {
		return;
	}

	child->SetViewport( m_viewport );
}

void Viewport::UpdateView() {
	m_viewport->source_origin.x = std::floor( m_horizontal_adjustment->GetValue() + .5f );
	m_viewport->source_origin.y = std::floor( m_vertical_adjustment->GetValue() + .5f );
}

}
