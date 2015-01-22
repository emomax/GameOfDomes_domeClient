package geometry
{
	import flash.display.Sprite;
	import flash.geom.Rectangle;
	import starling.display.Image;
	import starling.textures.Texture;
	
	import starling.display.Image;

	public class DrawRect extends starling.display.Sprite {
		
		public function DrawRect(r:Rectangle) {
			
			var rect:flash.display.Sprite = new flash.display.Sprite();
			rect.graphics.lineStyle(2,0xff0000,1);
			rect.graphics.drawRect(0,0,r.width,r.height);
			rect.graphics.endFill();
			
			var bmd:flash.display.BitmapData = new flash.display.BitmapData(rect.width, rect.height, true, 0x000000);
			bmd.draw(rect);
			
			var img:Image = new starling.display.Image(Texture.fromBitmapData(bmd, false, false))
			
			img.x = r.x;
			img.y = r.y;
			addChild(img);
		}
	}
}