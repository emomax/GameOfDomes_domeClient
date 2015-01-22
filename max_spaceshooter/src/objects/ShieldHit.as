package objects
{
	import starling.display.Image;
	import starling.display.Sprite;
	import starling.events.Event;
	
	//import debugger.Debug;
	
	public class ShieldHit extends Sprite {
		
		private var art:Image = new Image(Assets.getAtlas().getTexture("shieldHit"));
		private var _alpha:Number = 1;
		private var stageRef:Sprite;
		
		public function ShieldHit(s:Sprite) : void { 
			//Debug.INFO("Shield was hit! Time to shine!", this);
			
			this.stageRef = s;
			
			addChild(art);
			this.addEventListener(Event.ENTER_FRAME, fade); 
		}
		
		private function fade(e:Event) : void {
			_alpha -= 0.05;
			
			if (_alpha == 0) remove();
			
			art.alpha = _alpha;
		}
		
		private function remove() : void {
			this.removeEventListener(Event.ENTER_FRAME, fade);
			this.removeChild(art);
			
			if (stageRef.contains(this)) {
				stageRef.removeChild(this);
			}
		}
	}
}