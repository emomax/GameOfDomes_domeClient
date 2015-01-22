package objects
{
	public interface ENEMY
	{
		function get target() : Ship;
		function set target(s:Ship) : void;
		
		function clean() : void;
	}
}