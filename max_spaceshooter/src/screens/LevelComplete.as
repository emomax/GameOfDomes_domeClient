package screens {
	import starling.display.Sprite;
	import starling.display.Image;
	import starling.events.Event;
	
	import Assets;
	
	public class LevelComplete extends Sprite {
		
		private var bg:Image = new Image(Assets.getTexture("LevelCompleteImage"));
		
		
		public function LevelComplete() {
			super();
			this.addEventListener(starling.events.Event.ADDED_TO_STAGE, onAddedToStage);
		}
		
		private function onAddedToStage() : void {
			this.removeEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
			init();
		}
		
		private function init() : void {
			this.addChild(bg);
		}
	}
}