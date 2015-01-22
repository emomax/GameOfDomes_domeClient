package screens {
	
	import starling.display.Button;
	import starling.display.Image;
	import starling.display.Sprite;
	import starling.events.Event;
	import starling.events.TouchEvent;
	
	import events.NavigationEvent;
	
	public class Menu extends Sprite {
		
		private var bg:Image = new Image(Assets.getTexture("MenuImage"));
		private var startButton:Button;
		
		public function Menu() : void{
			super();
			this.addEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
		}
		
		private function onAddedToStage() : void {
			this.removeEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
			
			startButton = new Button(Assets.getAtlas().getTexture("startGameBtn"));
			startButton.x = stage.stageWidth / 2 - startButton.width / 2;
			startButton.y = 430;
			
			this.addChild(bg);
			this.addChild(startButton);
			startButton.addEventListener(Event.TRIGGERED, onClick);
		}
		
		private function onClick(e:Event) : void {
			
			this.removeEventListener(TouchEvent.TOUCH, onClick);
			trace("Clicked!");
			this.dispatchEvent(new NavigationEvent(NavigationEvent.CHANGE_SCREEN, {id: "lvlSelection"}, true));
		}
	}
}