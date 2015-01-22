package objects {

	import starling.display.Image;
	import starling.display.Sprite;
	
	public class Bar extends Sprite {
		
		private var bg:Image = new Image(Assets.getAtlas().getTexture("bar_bg"));
		public var bar:Image = new Image(Assets.getAtlas().getTexture("bar"));
		
		public function Bar() {
			super();
			this.addChild(bg);
			this.addChild(bar)
			
			bar.x = 1;
			bar.y = 1;
		}
	}
}